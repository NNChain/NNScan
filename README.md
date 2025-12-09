# Docker Setup Guide for NNScan Frontend

This guide will help you set up the NNScan frontend application using Docker and configure nginx or Apache2 as a reverse proxy.

## Prerequisites

- Docker installed on your system
- Docker Compose (optional, but recommended)
- nginx or Apache2 installed
- Built frontend files in the `dist/` directory

## Installation Location

**Important:** This guide assumes your project is located at `/var/www/nnscan`. If you've installed it in a different location, replace `/var/www/nnscan` with your actual project path throughout this guide.

For example, if your project is at `/home/user/nnscan` or `/opt/nnscan`, use that path instead.

**Quick Reference:** Set a variable for easier use:
```bash
export PROJECT_DIR="/var/www/nnscan"  # Change this to your actual path
cd $PROJECT_DIR
```

## Quick Start

### 1. Build the Frontend (if not already built)

```bash
cd /var/www/nnscan  # Replace with your actual project path
yarn install
yarn build
```

This will create the `dist/` directory with all static files.

### 2. Build Docker Image

```bash
cd /var/www/nnscan  # Replace with your actual project path
docker build -t nnscan-frontend:latest .
```

### 3. Run Docker Container

```bash
docker run -d \
  --name nnscan-frontend \
  --restart unless-stopped \
  -p 8080:80 \
  nnscan-frontend:latest
```

The container will serve the frontend on port 8080. We'll configure nginx/Apache2 to proxy to this port.

---

## Option A: nginx Setup

### Step 1: Create nginx Configuration

Create a new nginx configuration file:

```bash
sudo nano /etc/nginx/sites-available/nnscan-docker
```

Add the following configuration:

```nginx
# NNScan Frontend - Docker Proxy Configuration
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

**Note:** Replace `server_name _;` with your actual domain name if you have one (e.g., `server_name example.com www.example.com;`).

### Step 2: Enable the Configuration

```bash
# Create symlink to enable the site
sudo ln -s /etc/nginx/sites-available/nnscan-docker /etc/nginx/sites-enabled/nnscan-docker

# Remove default site if it conflicts (optional)
sudo rm /etc/nginx/sites-enabled/default
```

### Step 3: Test and Reload nginx

```bash
# Test nginx configuration
sudo nginx -t

# If test passes, reload nginx
sudo systemctl reload nginx
```

### Step 4: Verify Setup

```bash
# Check if Docker container is running
docker ps | grep nnscan-frontend

# Test the site
curl http://localhost/

# Check nginx status
sudo systemctl status nginx
```

---

## Option B: Apache2 Setup

### Step 1: Enable Required Apache Modules

```bash
sudo a2enmod proxy
sudo a2enmod proxy_http
sudo a2enmod headers
sudo a2enmod rewrite
```

### Step 2: Create Apache Configuration

Create a new Apache configuration file:

```bash
sudo nano /etc/apache2/sites-available/nnscan-docker.conf
```

Add the following configuration:

```apache
<VirtualHost *:80>
    ServerName _  # Replace with your domain name if you have one
    ServerAdmin admin@example.com

    # Proxy to Docker container
    ProxyPreserveHost On
    ProxyPass / http://127.0.0.1:8080/
    ProxyPassReverse / http://127.0.0.1:8080/

    # WebSocket support
    RewriteEngine on
    RewriteCond %{HTTP:Upgrade} websocket [NC]
    RewriteCond %{HTTP:Connection} upgrade [NC]
    RewriteRule ^/?(.*) "ws://127.0.0.1:8080/$1" [P,L]

    # Headers
    RequestHeader set X-Forwarded-Proto "http"
    RequestHeader set X-Real-IP %{REMOTE_ADDR}s

    # Logging
    ErrorLog ${APACHE_LOG_DIR}/nnscan-error.log
    CustomLog ${APACHE_LOG_DIR}/nnscan-access.log combined
</VirtualHost>
```

**Note:** Replace `ServerName _` with your actual domain name if you have one.

### Step 3: Enable the Site

```bash
# Enable the site
sudo a2ensite nnscan-docker.conf

# Disable default site if it conflicts (optional)
sudo a2dissite 000-default.conf
```

### Step 4: Test and Reload Apache

```bash
# Test Apache configuration
sudo apache2ctl configtest

# If test passes, reload Apache
sudo systemctl reload apache2
```

### Step 5: Verify Setup

```bash
# Check if Docker container is running
docker ps | grep nnscan-frontend

# Test the site
curl http://localhost/

# Check Apache status
sudo systemctl status apache2
```

---

## Docker Compose Setup (Alternative)

If you prefer using Docker Compose, create a `docker-compose.yml` file:

```yaml
version: '3.8'

services:
  frontend:
    build:
      context: .
      dockerfile: Dockerfile
    container_name: nnscan-frontend
    ports:
      - "8080:80"
    restart: unless-stopped
    environment:
      - NODE_ENV=production
    healthcheck:
      test: ["CMD", "wget", "--quiet", "--tries=1", "--spider", "http://localhost/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s
```

Then run:

```bash
docker-compose up -d --build
```

---

## Useful Commands

### Docker Commands

```bash
# View container logs
docker logs -f nnscan-frontend

# Restart container
docker restart nnscan-frontend

# Stop container
docker stop nnscan-frontend

# Start container
docker start nnscan-frontend

# Remove container
docker rm nnscan-frontend

# Rebuild and restart (after code changes)
cd /var/www/nnscan  # Replace with your actual project path
yarn build
docker build -t nnscan-frontend:latest .
docker stop nnscan-frontend
docker rm nnscan-frontend
docker run -d --name nnscan-frontend --restart unless-stopped -p 8080:80 nnscan-frontend:latest
```

### nginx Commands

```bash
# Test configuration
sudo nginx -t

# Reload configuration
sudo systemctl reload nginx

# Restart nginx
sudo systemctl restart nginx

# View error logs
sudo tail -f /var/log/nginx/error.log

# View access logs
sudo tail -f /var/log/nginx/access.log
```

### Apache2 Commands

```bash
# Test configuration
sudo apache2ctl configtest

# Reload configuration
sudo systemctl reload apache2

# Restart Apache
sudo systemctl restart apache2

# View error logs
sudo tail -f /var/log/apache2/error.log

# View access logs
sudo tail -f /var/log/apache2/access.log
```

---

## Troubleshooting

### Issue: Blank Page or 502 Bad Gateway

**Solution:**
1. Check if Docker container is running:
   ```bash
   docker ps | grep nnscan-frontend
   ```

2. Check container logs:
   ```bash
   docker logs nnscan-frontend
   ```

3. Verify the container is accessible:
   ```bash
   curl http://localhost:8080/
   ```

4. If container is not running, restart it:
   ```bash
   docker start nnscan-frontend
   ```

### Issue: Port 8080 Already in Use

**Solution:**
Change the port mapping in the Docker run command:
```bash
docker run -d --name nnscan-frontend --restart unless-stopped -p 8081:80 nnscan-frontend:latest
```

Then update your nginx/Apache2 configuration to proxy to port 8081 instead of 8080.

### Issue: nginx/Apache2 Configuration Error

**Solution:**
1. Test the configuration:
   - nginx: `sudo nginx -t`
   - Apache2: `sudo apache2ctl configtest`

2. Check error logs:
   - nginx: `sudo tail -f /var/log/nginx/error.log`
   - Apache2: `sudo tail -f /var/log/apache2/error.log`

3. Ensure all required modules are enabled (Apache2):
   ```bash
   sudo a2enmod proxy proxy_http headers rewrite
   ```

### Issue: Assets Not Loading (404 errors)

**Solution:**
1. Verify the `dist/` folder exists and contains files:
   ```bash
   ls -la /var/www/nnscan/dist/  # Replace with your actual project path
   ```

2. Rebuild the frontend:
   ```bash
   cd /var/www/nnscan  # Replace with your actual project path
   yarn build
   ```

3. Rebuild the Docker image:
   ```bash
   docker build -t nnscan-frontend:latest .
   docker restart nnscan-frontend
   ```

### Issue: WebSocket Connections Failing

**Solution:**
Ensure your nginx/Apache2 configuration includes WebSocket support:

**nginx:** The configuration provided above already includes WebSocket support via the `Upgrade` and `Connection` headers.

**Apache2:** The configuration includes WebSocket rewrite rules. If issues persist, ensure the `rewrite` module is enabled:
```bash
sudo a2enmod rewrite
sudo systemctl reload apache2
```

---

## SSL/HTTPS Setup (Optional)

### Using Let's Encrypt with Certbot

#### For nginx:

```bash
# Install certbot
sudo apt-get update
sudo apt-get install certbot python3-certbot-nginx

# Get SSL certificate
sudo certbot --nginx -d yourdomain.com

# Certbot will automatically update your nginx configuration
```

#### For Apache2:

```bash
# Install certbot
sudo apt-get update
sudo apt-get install certbot python3-certbot-apache

# Get SSL certificate
sudo certbot --apache -d yourdomain.com

# Certbot will automatically update your Apache configuration
```

After SSL setup, update your nginx/Apache2 configuration to redirect HTTP to HTTPS and proxy to the Docker container on port 8080.

---

## Production Recommendations

1. **Use a domain name** instead of IP address in `server_name` directive
2. **Enable SSL/HTTPS** using Let's Encrypt
3. **Set up firewall rules** to only allow necessary ports (80, 443)
4. **Monitor container health** using Docker health checks
5. **Set up log rotation** for nginx/Apache2 logs
6. **Use Docker Compose** for easier management
7. **Set up automated backups** of your configuration files
8. **Enable rate limiting** in nginx/Apache2 to prevent abuse

---

## File Structure

Your project directory (e.g., `/var/www/nnscan` or wherever you installed it) should contain:

```
<your-project-directory>/
├── Dockerfile              # Docker build configuration
├── docker-compose.yml      # Docker Compose configuration (optional)
├── nginx.conf              # nginx config inside Docker container
├── .dockerignore           # Files to exclude from Docker build
├── dist/                   # Built frontend files (created by yarn build)
│   ├── index.html
│   └── assets/
└── docker-setup.sh         # Automated setup script (optional)
```

**Note:** Replace `<your-project-directory>` with your actual installation path (e.g., `/var/www/nnscan`, `/home/user/nnscan`, `/opt/nnscan`, etc.).

---

## Support

If you encounter any issues:

1. Check Docker container logs: `docker logs nnscan-frontend`
2. Check nginx/Apache2 error logs
3. Verify the `dist/` folder contains all necessary files in your project directory
4. Ensure port 8080 is not being used by another service
5. Verify nginx/Apache2 configuration syntax
6. **Double-check all paths** - Make sure you're using the correct project directory path throughout the guide

---

## Quick Reference

**Default Ports:**
- Docker container: 8080 (internal: 80)
- nginx/Apache2: 80 (HTTP), 443 (HTTPS)

**Key Files:**
- nginx config: `/etc/nginx/sites-available/nnscan-docker`
- Apache config: `/etc/apache2/sites-available/nnscan-docker.conf`
- Docker image: `nnscan-frontend:latest`
- Container name: `nnscan-frontend`

**Health Check:**
```bash
curl http://localhost/health
# Should return: healthy
```
