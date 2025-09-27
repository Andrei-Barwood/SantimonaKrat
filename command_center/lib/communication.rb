# command_center/lib/communication.rb
require 'socket'
require 'json'

class Communication
  def initialize
    @server = UDPServer.new('0.0.0.0', 8888)
    @message_handler = MessageHandler.new
    start_server
  end

  def send_message(drone_id, message)
    # Serialize message using shared format
    json_message = JSON.generate(message)
    # Send via UDP to drone
    # Implementation details...
  end

  private

  def start_server
    Thread.new do
      loop do
        data, client = @server.recvfrom(8192)
        handle_message(data, client)
      end
    end
  end

  def handle_message(data, client)
    message = JSON.parse(data)
    @message_handler.process(message)
  end
end