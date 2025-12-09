# Deployment Guide for nnscan-cosmos on a New VPS

This guide will help you deploy the nnscan-cosmos application on a new VPS server.

## Prerequisites

- A VPS with Ubuntu/Debian Linux
- Root or sudo access
- Basic knowledge of Linux commands

## Step 1: Transfer the Application

### Option A: Using the zip file (Recommended)

1. **On the new VPS, download the zip file:**
   ```bash
   cd /var/www
   wget http://YOUR_CURRENT_VPS_IP/nnscan-cosmos.zip
   # Or use curl:
   # curl -O http://YOUR_CURRENT_VPS_IP/nnscan-cosmos.zip
   ```

2. **Extract the zip file:**
   ```bash
   unzip nnscan-cosmos.zip
   cd nnscan-cosmos-main
   ```

### Option B: Using SCP (from your local machine)

```bash
scp nnscan-cosmos.zip user@new-vps-ip:/var/www/
ssh user@new-vps-ip
cd /var/www
unzip nnscan-cosmos.zip
cd nnscan-cosmos-main
```

## Step 2: Install Required Software

### Install Node.js and Yarn

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install Node.js (using NodeSource repository for Node 16+)
curl -fsSL https://deb.nodesource.com/setup_16.x | sudo -E bash -
sudo apt install -y nodejs

# Install Yarn
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
sudo apt update && sudo apt install -y yarn

# Verify installations
node --version
yarn --version
```

### Install Docker (Optional - for Docker deployment)

```bash
# Install Docker
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh

# Install Docker Compose
sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

# Verify Docker
docker --version
docker-compose --version
```

### Install nginx

```bash
sudo apt install -y nginx
sudo systemctl enable nginx
sudo systemctl start nginx
```

## Step 3: Build the Application

```bash
cd /var/www/nnscan-cosmos-main

# Install dependencies
yarn install

# Build the application (this creates the dist/ folder)
yarn build

# Verify the dist folder was created
ls -la dist/
```

**Note:** The build process may take several minutes. Make sure you have enough disk space.

## Step 4: Deploy the Application

You have three deployment options:

---

## Option A: Direct nginx Deployment (Simplest)

This is similar to your current setup - nginx serves the built files directly.

### 1. Configure nginx

```bash
sudo nano /etc/nginx/sites-available/nnscan
```

Add this configuration:

```nginx
server {
    listen 80;
    server_name _;  # Replace with your domain name if you have one
    
    root /var/www/nnscan-cosmos-main/dist;
    index index.html;
    
    # SPA routing - all routes should serve index.html
    location / {
        try_files $uri $uri/ /index.html =404;
    }
    
    # Exclude API and GraphQL endpoints from SPA routing
    location ~ ^/(rest|bandsv|faucet|v1)/ {
        return 404;
    }
    
    # Cache static assets
    location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
    
    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;
    
    # Gzip compression
    gzip on;
    gzip_vary on;
    gzip_min_length 1024;
    gzip_types text/plain text/css text/xml text/javascript application/x-javascript application/xml+rss application/json application/javascript;
}
```

### 2. Enable the site

```bash
# Create symlink
sudo ln -s /etc/nginx/sites-available/nnscan /etc/nginx/sites-enabled/nnscan

# Remove default site if it conflicts
sudo rm /etc/nginx/sites-enabled/default

# Test configuration
sudo nginx -t

# Reload nginx
sudo systemctl reload nginx
```

### 3. Verify

```bash
curl http://localhost/
```

---

## Option B: Docker Deployment (Recommended for Production)

### 1. Build Docker image

```bash
cd /var/www/nnscan-cosmos-main
docker build -t nnscan-frontend:latest .
```

### 2. Run Docker container

```bash
docker run -d \
  --name nnscan-frontend \
  --restart unless-stopped \
  -p 8080:80 \
  nnscan-frontend:latest
```

### 3. Configure nginx as reverse proxy

```bash
sudo nano /etc/nginx/sites-available/nnscan-docker
```

Add this configuration:

```nginx
server {
    listen 80;
    server_name _;  # Replace with your domain name if you have one

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

    # Health check endpoint
    location /health {
        proxy_pass http://127.0.0.1:8080/health;
        access_log off;
    }
}
```

### 4. Enable and reload

```bash
sudo ln -s /etc/nginx/sites-available/nnscan-docker /etc/nginx/sites-enabled/nnscan-docker
sudo rm /etc/nginx/sites-enabled/default
sudo nginx -t
sudo systemctl reload nginx
```

---

## Option C: Docker Compose Deployment

### 1. Use Docker Compose

```bash
cd /var/www/nnscan-cosmos-main
docker-compose up -d --build
```

### 2. Configure nginx (same as Option B, step 3-4)

---

## Step 5: Configure Firewall

```bash
# Allow HTTP and HTTPS
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp

# Enable firewall (if not already enabled)
sudo ufw enable

# Check status
sudo ufw status
```

## Step 6: Set Up SSL/HTTPS (Optional but Recommended)

### Using Let's Encrypt with Certbot

```bash
# Install certbot
sudo apt install -y certbot python3-certbot-nginx

# Get SSL certificate (replace with your domain)
sudo certbot --nginx -d yourdomain.com

# Certbot will automatically configure nginx for HTTPS
```

## Step 7: Verify Deployment

1. **Check nginx status:**
   ```bash
   sudo systemctl status nginx
   ```

2. **Check Docker container (if using Docker):**
   ```bash
   docker ps
   docker logs nnscan-frontend
   ```

3. **Test the application:**
   ```bash
   curl http://localhost/
   # Or visit http://YOUR_VPS_IP in a browser
   ```

4. **Check health endpoint:**
   ```bash
   curl http://localhost/health
   ```

## Updating the Application

When you need to update the application:

### For Direct nginx Deployment:

```bash
cd /var/www/nnscan-cosmos-main
git pull  # If using git, or extract new zip file
yarn install
yarn build
sudo systemctl reload nginx
```

### For Docker Deployment:

```bash
cd /var/www/nnscan-cosmos-main
git pull  # If using git, or extract new zip file
yarn install
yarn build
docker build -t nnscan-frontend:latest .
docker stop nnscan-frontend
docker rm nnscan-frontend
docker run -d --name nnscan-frontend --restart unless-stopped -p 8080:80 nnscan-frontend:latest
```

### For Docker Compose:

```bash
cd /var/www/nnscan-cosmos-main
git pull  # If using git, or extract new zip file
yarn install
yarn build
docker-compose up -d --build
```

## Troubleshooting

### Issue: Build fails

**Solution:**
- Ensure Node.js version 16+ is installed
- Check disk space: `df -h`
- Clear yarn cache: `yarn cache clean`
- Try removing node_modules and reinstalling: `rm -rf node_modules && yarn install`

### Issue: nginx 502 Bad Gateway

**Solution:**
- Check if dist folder exists: `ls -la /var/www/nnscan-cosmos-main/dist`
- Verify Docker container is running: `docker ps`
- Check nginx error logs: `sudo tail -f /var/log/nginx/error.log`
- Check Docker logs: `docker logs nnscan-frontend`

### Issue: Blank page

**Solution:**
- Verify dist folder has files: `ls -la dist/`
- Check browser console for errors
- Verify nginx configuration: `sudo nginx -t`
- Check file permissions: `sudo chown -R www-data:www-data /var/www/nnscan-cosmos-main/dist`

### Issue: Port already in use

**Solution:**
- Check what's using the port: `sudo lsof -i :80` or `sudo netstat -tulpn | grep :80`
- Change the port in Docker or nginx configuration

## Useful Commands

```bash
# View nginx logs
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log

# View Docker logs
docker logs -f nnscan-frontend

# Restart services
sudo systemctl restart nginx
docker restart nnscan-frontend

# Check disk space
df -h

# Check memory usage
free -h

# Check running processes
ps aux | grep nginx
docker ps
```

## Production Checklist

- [ ] Application built successfully
- [ ] nginx/Docker configured and running
- [ ] Firewall configured
- [ ] SSL certificate installed (if using domain)
- [ ] Application accessible via IP/domain
- [ ] Health check endpoint working
- [ ] Logs are being generated
- [ ] Auto-restart configured (systemd/Docker restart policies)

## Support

If you encounter issues:
1. Check the logs (nginx and Docker)
2. Verify all prerequisites are installed
3. Ensure file permissions are correct
4. Verify network connectivity
5. Check disk space and memory

