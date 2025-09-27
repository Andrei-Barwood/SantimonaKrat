# command_center/lib/drone_manager.rb
class DroneManager
  def initialize
    @drones = {}
    @communication = Communication.new
    @building_plans = {}
  end

  def connect_to_drone(drone_id, ip_address)
    @drones[drone_id] = {
      id: drone_id,
      ip: ip_address,
      status: :disconnected,
      last_seen: Time.now
    }
  end

  def send_command(drone_id, command, params = {})
    message = {
      type: 'command',
      drone_id: drone_id,
      command: command,
      params: params,
      timestamp: Time.now.to_i
    }
    
    @communication.send_message(drone_id, message)
  end

  def get_drone_status(drone_id)
    # Send status request to drone
    send_command(drone_id, 'status_request')
    # Response will be handled asynchronously
  end
end