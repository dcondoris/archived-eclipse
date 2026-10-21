# Using Alpine Linux as the base image because of it's light weight
FROM alpine:latest

# Installing only the necessary C compiler as well as make
RUN apk update &&\
    apk add --no-cache gcc make &&\
    rm -rf /var/cache/apk/*

# Creating and moving to the directory that will be used for the code
WORKDIR /code

# Expose port 8080
EXPOSE 8080

# Using standard shell as the entrypoint
CMD ["sh"]