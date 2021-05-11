
export default class Position {
	latitude: number;
	longitude: number;
	altitude: number;

	constructor(latitude: number, longitude: number, altitude: number) {
		this.latitude = latitude
		this.longitude = longitude
		this.altitude = altitude
	}

	toString() {
		return `{ ${this.latitude}; ${this.longitude}; ${this.altitude} }`
	}
}
