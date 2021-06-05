export enum MavGeneratorStatusFlag {
	MAV_GENERATOR_STATUS_FLAG_OFF = 1, // Generator is off.
	MAV_GENERATOR_STATUS_FLAG_READY = 2, // Generator is ready to start generating power.
	MAV_GENERATOR_STATUS_FLAG_GENERATING = 4, // Generator is generating power.
	MAV_GENERATOR_STATUS_FLAG_CHARGING = 8, // Generator is charging the batteries (generating enough power to charge and provide the load).
	MAV_GENERATOR_STATUS_FLAG_REDUCED_POWER = 16, // Generator is operating at a reduced maximum power.
	MAV_GENERATOR_STATUS_FLAG_MAXPOWER = 32, // Generator is providing the maximum output.
	MAV_GENERATOR_STATUS_FLAG_OVERTEMP_WARNING = 64, // Generator is near the maximum operating temperature, cooling is insufficient.
	MAV_GENERATOR_STATUS_FLAG_OVERTEMP_FAULT = 128, // Generator hit the maximum operating temperature and shutdown.
	MAV_GENERATOR_STATUS_FLAG_ELECTRONICS_OVERTEMP_WARNING = 256, // Power electronics are near the maximum operating temperature, cooling is insufficient.
	MAV_GENERATOR_STATUS_FLAG_ELECTRONICS_OVERTEMP_FAULT = 512, // Power electronics hit the maximum operating temperature and shutdown.
	MAV_GENERATOR_STATUS_FLAG_ELECTRONICS_FAULT = 1024, // Power electronics experienced a fault and shutdown.
	MAV_GENERATOR_STATUS_FLAG_POWERSOURCE_FAULT = 2048, // The power source supplying the generator failed e.g. mechanical generator stopped, tether is no longer providing power, solar cell is in shade, hydrogen reaction no longer happening.
	MAV_GENERATOR_STATUS_FLAG_COMMUNICATION_WARNING = 4096, // Generator controller having communication problems.
	MAV_GENERATOR_STATUS_FLAG_COOLING_WARNING = 8192, // Power electronic or generator cooling system error.
	MAV_GENERATOR_STATUS_FLAG_POWER_RAIL_FAULT = 16384, // Generator controller power rail experienced a fault.
	MAV_GENERATOR_STATUS_FLAG_OVERCURRENT_FAULT = 32768, // Generator controller exceeded the overcurrent threshold and shutdown to prevent damage.
	MAV_GENERATOR_STATUS_FLAG_BATTERY_OVERCHARGE_CURRENT_FAULT = 65536, // Generator controller detected a high current going into the batteries and shutdown to prevent battery damage.
	MAV_GENERATOR_STATUS_FLAG_OVERVOLTAGE_FAULT = 131072, // Generator controller exceeded it's overvoltage threshold and shutdown to prevent it exceeding the voltage rating.
	MAV_GENERATOR_STATUS_FLAG_BATTERY_UNDERVOLT_FAULT = 262144, // Batteries are under voltage (generator will not start).
	MAV_GENERATOR_STATUS_FLAG_START_INHIBITED = 524288, // Generator start is inhibited by e.g. a safety switch.
	MAV_GENERATOR_STATUS_FLAG_MAINTENANCE_REQUIRED = 1048576, // Generator requires maintenance.
	MAV_GENERATOR_STATUS_FLAG_WARMING_UP = 2097152, // Generator is not ready to generate yet.
	MAV_GENERATOR_STATUS_FLAG_IDLE = 4194304, // Generator is idle.
	MAV_GENERATOR_STATUS_FLAG_ENUM_END = 4194305, // 
}