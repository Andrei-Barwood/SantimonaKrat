// DroneControlService.java
public class DroneControlService extends Service {
    private DroneManager droneManager;
    private Handler mainHandler;
    
    @Override
    public void onCreate() {
        super.onCreate();
        droneManager = new DroneManager();
        mainHandler = new Handler(Looper.getMainLooper());
        
        // Initialize 50 drones
        droneManager.initializeDrones(50);
    }
    
    public class DroneControlBinder extends Binder {
        public DroneControlService getService() {
            return DroneControlService.this;
        }
    }
    
    @Override
    public IBinder onBind(Intent intent) {
        return new DroneControlBinder();
    }
    
    public void sendBuildingPlan(BuildingPlan plan) {
        droneManager.assignBuildingPlan(plan);
    }
    
    public void startConstruction() {
        droneManager.startConstruction();
    }
    
    public List<DroneStatus> getDroneStatuses() {
        return droneManager.getDroneStatuses();
    }
}