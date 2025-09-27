#!/bin/bash
# scripts/deploy_command_center.sh

echo "Deploying Ruby command center..."
cd command_center

# Install dependencies
bundle install

# Run database migrations
rake db:migrate

# Start the command center
bundle exec thin start -p 3000

echo "Command center deployed and running on port 3000!"