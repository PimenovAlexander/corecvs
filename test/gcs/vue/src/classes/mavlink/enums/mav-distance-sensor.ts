export enum MavDistanceSensor {
	MAV_DISTANCE_SENSOR_LASER = 0, // Laser rangefinder, e.g. LightWare SF02/F or PulsedLight units
	MAV_DISTANCE_SENSOR_ULTRASOUND = 1, // Ultrasound rangefinder, e.g. MaxBotix units
	MAV_DISTANCE_SENSOR_INFRARED = 2, // Infrared rangefinder, e.g. Sharp units
	MAV_DISTANCE_SENSOR_RADAR = 3, // Radar type, e.g. uLanding units
	MAV_DISTANCE_SENSOR_UNKNOWN = 4, // Broken or unknown type, e.g. analog units
	MAV_DISTANCE_SENSOR_ENUM_END = 5, // 
}