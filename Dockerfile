# Simple nginx server for pre-built frontend
FROM nginx:alpine

# Copy built files from host (assumes dist/ folder exists)
COPY dist/ /usr/share/nginx/html/

# Copy nginx configuration
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Expose port 80
EXPOSE 80

# Start nginx
CMD ["nginx", "-g", "daemon off;"]

