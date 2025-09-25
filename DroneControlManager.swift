// DroneControlManager.swift
import Foundation
import CoreLocation

class DroneControlManager: ObservableObject {
    @Published var droneStatuses: [DroneStatus] = []
    private var drones: [ConstructionDrone] = []
    
    func initializeDrones(count: Int) {
        for i in 0..<count {
            let drone = ConstructionDrone(droneId: "CONSTRUCT_\(String(format: "%02d", i))")
            drones.append(drone)
        }
    }
    
    func sendBuildingPlan(_ plan: BuildingPlan) {
        // Distribute building plan to drones
        DispatchQueue.global(qos: .background).async {
            self.distributePlanToDrones(plan: plan)
        }
    }
    
    func startConstruction() {
        for drone in drones {
            DispatchQueue.global(qos: .background).async {
                drone.startConstruction()
            }
        }
    }
    
    private func distributePlanToDrones(plan: BuildingPlan) {
        let componentsPerDrone = plan.components.count / drones.count
        var startIndex = 0
        
        for drone in drones {
            let endIndex = min(startIndex + componentsPerDrone, plan.components.count)
            let dronePlan = Array(plan.components[startIndex..<endIndex])
            drone.assignPlan(components: dronePlan)
            startIndex = endIndex
        }
    }
}