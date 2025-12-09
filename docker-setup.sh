#!/bin/bash

# Script to migrate from direct nginx serving to Docker container

set -e

echo "🚀 Setting up Docker for NNScan Frontend"
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo "❌ Please run as root (use sudo)"
    exit 1
fi

# Step 1: Install Docker if not installed
if ! command -v docker &> /dev/null; then
    echo "📦 Installing Docker..."
    apt-get update
    apt-get install -y docker.io docker-compose
    systemctl enable docker
    systemctl start docker
    echo "✅ Docker installed"
else
    echo "✅ Docker already installed"
fi

# Step 2: Ensure dist folder exists
echo ""
echo "📦 Checking for built files..."
cd /var/www/nnscan-cosmos-main
if [ ! -d "dist" ] || [ -z "$(ls -A dist)" ]; then
    echo "⚠️  dist folder is empty or missing. Building application..."
    yarn build
    if [ $? -ne 0 ]; then
        echo "❌ Build failed. Please fix build errors first."
        exit 1
    fi
else
    echo "✅ dist folder found"
fi

# Step 3: Build Docker image
echo ""
echo "🔨 Building Docker image..."
docker build -t nnscan-frontend:latest .

# Step 3: Stop existing nginx service for this app (optional - we'll proxy instead)
echo ""
echo "📝 Current nginx configuration will be updated to proxy to Docker"
echo "   (Nginx will stay running on port 80 and proxy to Docker on port 8080)"

# Step 4: Create nginx proxy configuration
echo ""
echo "📝 Creating nginx proxy configuration..."
cat > /etc/nginx/sites-available/nnscan-docker << 'EOF'
# NNScan Frontend - Docker Proxy Configuration
server {
    listen 80;
    server_name _;

    # Proxy to Docker container
    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_cache_bypass $http_upgrade;
        
        # Timeouts
        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;
    }

    # Health check
    location /health {
        proxy_pass http://127.0.0.1:8080/health;
        access_log off;
    }
}
EOF

# Step 5: Backup and disable old nginx config
echo ""
echo "📝 Updating nginx configuration..."
if [ -f /etc/nginx/sites-enabled/default ]; then
    rm /etc/nginx/sites-enabled/default
fi

# Backup old config if exists
if [ -f /etc/nginx/sites-enabled/nnscan ]; then
    mv /etc/nginx/sites-enabled/nnscan /etc/nginx/sites-enabled/nnscan.backup
fi

# Enable new config
ln -sf /etc/nginx/sites-available/nnscan-docker /etc/nginx/sites-enabled/nnscan-docker

# Step 6: Enable new configuration
ln -sf /etc/nginx/sites-available/nnscan-docker /etc/nginx/sites-enabled/nnscan-docker

# Step 7: Test nginx configuration
echo ""
echo "🧪 Testing nginx configuration..."
nginx -t

# Step 8: Start Docker container
echo ""
echo "🐳 Starting Docker container..."
docker stop nnscan-frontend 2>/dev/null || true
docker rm nnscan-frontend 2>/dev/null || true
docker run -d \
    --name nnscan-frontend \
    --restart unless-stopped \
    -p 8080:80 \
    nnscan-frontend:latest

# Step 9: Reload nginx
echo ""
echo "🔄 Reloading nginx..."
systemctl reload nginx

# Step 10: Wait for container to be ready
echo ""
echo "⏳ Waiting for container to be ready..."
sleep 5

# Step 11: Check status
echo ""
echo "✅ Setup complete!"
echo ""
echo "📊 Status:"
docker ps | grep nnscan-frontend || echo "⚠️  Container not running"
echo ""
echo "🌐 Application should be available at:"
echo "   http://$(hostname -I | awk '{print $1}')"
echo ""
echo "📝 Useful commands:"
echo "   View logs:        docker logs -f nnscan-frontend"
echo "   Restart:          docker restart nnscan-frontend"
echo "   Stop:             docker stop nnscan-frontend"
echo "   Start:            docker start nnscan-frontend"
echo "   Rebuild:          cd /var/www/nnscan-cosmos-main && docker build -t nnscan-frontend:latest . && docker restart nnscan-frontend"

