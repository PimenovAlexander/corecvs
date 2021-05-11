
import Position from './Position'

export default class Waypoint {

	static OPTIONS = {
		hold: 1,
		acceptRadius: 2,
		passRadius: 3,
		yaw: 4,
		latitude: 5,
		longitude: 6,
		altitude: 7
	}

	// MAV_CMD_NAV_WAYPOINT
	/*
	Param (:Label)			Values		Units
	1: Hold					min:0		s
	2: Accept Radius		min:0		m
	3: Pass Radius						m
	4: Yaw								deg
	5: Latitude
	6: Longitude
	7: Altitude	Altitude				m
	*/

	static defaultOptions = {
		hold: 0,
		yaw: 0,
		acceptRadius: 0,
		passRadius: 0
	}

	position: Position;
	id: number;

	hold: number;
	yaw: number;
	acceptRadius: number;
	passRadius: number;

	static curId = 0;

	constructor(position: Position) {
		this.position = position
		this.id = Waypoint.getId();

		this.hold =			Waypoint.defaultOptions.hold
		this.yaw =			Waypoint.defaultOptions.yaw
		this.acceptRadius =	Waypoint.defaultOptions.acceptRadius
		this.passRadius =	Waypoint.defaultOptions.passRadius
	}

	toString() {
		return `Waypoint: ${this.position}`
	}

	static getId() {
		return Waypoint.curId++;
	}

	isCorrect() {
		return (this.errorValues().length == 0)
	}

	errorValues() {
		const vars = [
			this.hold,
			this.acceptRadius,
			this.passRadius,
			this.yaw,

			this.position.latitude,
			this.position.longitude,
			this.position.altitude
		]

		const res: number[] = []

		vars.map((number, index) => ({
			number, index
		})).filter(item => {
			return isNaN(item.number)
		}).forEach(item => {
			res.push(item.index + 1)
		})

		if ((!(res.includes(Waypoint.OPTIONS.latitude)) &&
			(vars[Waypoint.OPTIONS.latitude - 1] < -90) || vars[Waypoint.OPTIONS.latitude - 1] > 90)) {
			res.push(Waypoint.OPTIONS.latitude)
			console.log(vars[Waypoint.OPTIONS.latitude - 1])
		}

		if ((!(res.includes(Waypoint.OPTIONS.longitude)) &&
			(vars[Waypoint.OPTIONS.longitude - 1] < -180) || vars[Waypoint.OPTIONS.longitude - 1] > 180)) {
			res.push(Waypoint.OPTIONS.longitude)
		}

		if ((!(res.includes(Waypoint.OPTIONS.hold))) && vars[Waypoint.OPTIONS.hold - 1] < 0) {
			res.push(Waypoint.OPTIONS.hold)
		}

		if ((!(res.includes(Waypoint.OPTIONS.acceptRadius))) && vars[Waypoint.OPTIONS.acceptRadius - 1] < 0) {
			res.push(Waypoint.OPTIONS.acceptRadius)
		}

		return res
	}

	toURI(): string {
		return `
			lat=${encodeURIComponent(this.position.latitude)}&
			lon=${encodeURIComponent(this.position.longitude)}&
			alt=${encodeURIComponent(this.position.altitude)}&
			id=${encodeURIComponent(this.id)}&
			hold=${encodeURIComponent(this.hold)}&
			ar=${encodeURIComponent(this.acceptRadius)}&
			pr=${encodeURIComponent(this.passRadius)}&
			yaw=${encodeURIComponent(this.yaw)}
		`
	}
}
