#!/bin/bash

# Prompt for database details
read -p "Enter Database IP: " DATABASE_IP
read -p "Enter Database Username: " DATABASE_USERNAME
read -s -p "Enter Database Password: " DATABASE_PASSWORD
echo
read -p "Enter Database Schema: " DATABASE_SCHEMA

# Create .env file and add database details
echo "DATABASE_IP=$DATABASE_IP" > .env
echo "DATABASE_USERNAME=$DATABASE_USERNAME" >> .env
echo "DATABASE_PASSWORD=$DATABASE_PASSWORD" >> .env
echo "DATABASE_SCHEMA=$DATABASE_SCHEMA" >> .env

echo ".env file has been created with the provided database details."
