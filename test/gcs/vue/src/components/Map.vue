
<template>
	<div id="mapComponent">
		<div class="map" id="map">

		</div>
		<div class="gps">
			<div class="property">
				<input id="latitude" type="text" class="green" @keyup="updatePosition()"/>
				<label>Latitude</label>
			</div>
			<div class="property">
				<input id="longitude" type="text" class="green" @keyup="updatePosition()"/>
				<label>Longitude</label>
			</div>
			<div class="property">
				<input id="viewSize" type="text" class="green" @keyup="updatePosition()"/>
				<label>Zoom</label>
			</div>
		</div>
	</div>
</template>

<script lang="ts">
import { Component, Prop, Vue } from 'vue-property-decorator';

import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

import Position from '../classes/Position';

// A component that displays a map of the area and waypoints as well as UAV's current position

@Component
export default class Map extends Vue {
	@Prop() private position!: Position;
	@Prop() private addMap!: (map: L.Map) => void;

	map: L.Map|undefined;
	x = 0
	y = 0
	z = 0

	copterMarker: L.Marker|undefined

	updatePosition(coordinates: number[] = []) {
		const latitude = document.getElementById('latitude') as HTMLInputElement
		const longitude = document.getElementById('longitude') as HTMLInputElement
		const viewSize = document.getElementById('viewSize') as HTMLInputElement

		if (coordinates.length == 0) {
			this.x = Number(latitude.value)
			this.y = Number(longitude.value)
			this.z = Number(viewSize.value)
		}
		if (coordinates.length == 3) {
			[ this.x, this.y, this.z ] = coordinates
			latitude.value = coordinates[0].toString()
			longitude.value = coordinates[1].toString()
			viewSize.value = coordinates[2].toString()
		}

		latitude.classList.replace(latitude.classList[0], isNaN(this.x) || latitude.value.trim() == '' ? 'red': 'green')
		longitude.classList.replace(longitude.classList[0], isNaN(this.y) || longitude.value.trim() == '' ? 'red': 'green')
		viewSize.classList.replace(viewSize.classList[0], isNaN(this.z) || viewSize.value.trim() == '' ? 'red': 'green')

		this.map!.flyTo([this.x, this.y], this.z);
	}

	mounted() {
		// Initializing a map that displays area around starting coordinates
		this.map = L.map('map').setView([this.position.latitude, this.position.longitude], 18);

		this.map.on('moveend', event => {
			const map = this.map!
			const pos = map.getCenter();
			if (this.x != pos.lat || this.y != pos.lng || this.z != map.getZoom()) {
				this.x = pos.lat
				this.y = pos.lng
				this.z = map.getZoom()
				this.updatePosition([this.x, this.y, this.z])
			}
		});

		const copterIcon = L.icon({
			iconUrl: 'https://www.freeiconspng.com/uploads/helicopter-icon-11.png',

			iconSize:     [50, 40], // size of the icon
			iconAnchor:   [25, 20], // point of the icon which will correspond to marker's location
		});
		
		this.copterMarker = L.marker([this.position.latitude, this.position.longitude], {icon: copterIcon});
		this.copterMarker!.addTo(this.map)

		this.addMap(this.map)

		L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
			attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
		}).addTo(this.map)

		// https://github.com/Leaflet/Leaflet/issues/6822

		const defaultMarker = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAApCAYAAADAk4LOAAAFgUlEQVR4Aa1XA5BjWRTN2oW17d3YaZtr2962HUzbDNpjszW24mRt28p47v7zq/bXZtrp/lWnXr337j3nPCe85NcypgSFdugCpW5YoDAMRaIMqRi6aKq5E3YqDQO3qAwjVWrD8Ncq/RBpykd8oZUb/kaJutow8r1aP9II0WmLKLIsJyv1w/kqw9Ch2MYdB++12Onxee/QMwvf4/Dk/Lfp/i4nxTXtOoQ4pW5Aj7wpici1A9erdAN2OH64x8OSP9j3Ft3b7aWkTg/Fm91siTra0f9on5sQr9INejH6CUUUpavjFNq1B+Oadhxmnfa8RfEmN8VNAsQhPqF55xHkMzz3jSmChWU6f7/XZKNH+9+hBLOHYozuKQPxyMPUKkrX/K0uWnfFaJGS1QPRtZsOPtr3NsW0uyh6NNCOkU3Yz+bXbT3I8G3xE5EXLXtCXbbqwCO9zPQYPRTZ5vIDXD7U+w7rFDEoUUf7ibHIR4y6bLVPXrz8JVZEql13trxwue/uDivd3fkWRbS6/IA2bID4uk0UpF1N8qLlbBlXs4Ee7HLTfV1j54APvODnSfOWBqtKVvjgLKzF5YdEk5ewRkGlK0i33Eofffc7HT56jD7/6U+qH3Cx7SBLNntH5YIPvODnyfIXZYRVDPqgHtLs5ABHD3YzLuespb7t79FY34DjMwrVrcTuwlT55YMPvOBnRrJ4VXTdNnYug5ucHLBjEpt30701A3Ts+HEa73u6dT3FNWwflY86eMHPk+Yu+i6pzUpRrW7SNDg5JHR4KapmM5Wv2E8Tfcb1HoqqHMHU+uWDD7zg54mz5/2BSnizi9T1Dg4QQXLToGNCkb6tb1NU+QAlGr1++eADrzhn/u8Q2YZhQVlZ5+CAOtqfbhmaUCS1ezNFVm2imDbPmPng5wmz+gwh+oHDce0eUtQ6OGDIyR0uUhUsoO3vfDmmgOezH0mZN59x7MBi++WDL1g/eEiU3avlidO671bkLfwbw5XV2P8Pzo0ydy4t2/0eu33xYSOMOD8hTf4CrBtGMSoXfPLchX+J0ruSePw3LZeK0juPJbYzrhkH0io7B3k164hiGvawhOKMLkrQLyVpZg8rHFW7E2uHOL888IBPlNZ1FPzstSJM694fWr6RwpvcJK60+0HCILTBzZLFNdtAzJaohze60T8qBzyh5ZuOg5e7uwQppofEmf2++DYvmySqGBuKaicF1blQjhuHdvCIMvp8whTTfZzI7RldpwtSzL+F1+wkdZ2TBOW2gIF88PBTzD/gpeREAMEbxnJcaJHNHrpzji0gQCS6hdkEeYt9DF/2qPcEC8RM28Hwmr3sdNyht00byAut2k3gufWNtgtOEOFGUwcXWNDbdNbpgBGxEvKkOQsxivJx33iow0Vw5S6SVTrpVq11ysA2Rp7gTfPfktc6zhtXBBC+adRLshf6sG2RfHPZ5EAc4sVZ83yCN00Fk/4kggu40ZTvIEm5g24qtU4KjBrx/BTTH8ifVASAG7gKrnWxJDcU7x8X6Ecczhm3o6YicvsLXWfh3Ch1W0k8x0nXF+0fFxgt4phz8QvypiwCCFKMqXCnqXExjq10beH+UUA7+nG6mdG/Pu0f3LgFcGrl2s0kNNjpmoJ9o4B29CMO8dMT4Q5ox8uitF6fqsrJOr8qnwNbRzv6hSnG5wP+64C7h9lp30hKNtKdWjtdkbuPA19nJ7Tz3zR/ibgARbhb4AlhavcBebmTHcFl2fvYEnW0ox9xMxKBS8btJ+KiEbq9zA4RthQXDhPa0T9TEe69gWupwc6uBUphquXgf+/FrIjweHQS4/pduMe5ERUMHUd9xv8ZR98CxkS4F2n3EUrUZ10EYNw7BWm9x1GiPssi3GgiGRDKWRYZfXlON+dfNbM+GgIwYdwAAAAASUVORK5CYII=';

		const defaultMarkerShadow = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACkAAAApCAQAAAACach9AAACMUlEQVR4Ae3ShY7jQBAE0Aoz/f9/HTMzhg1zrdKUrJbdx+Kd2nD8VNudfsL/Th///dyQN2TH6f3y/BGpC379rV+S+qqetBOxImNQXL8JCAr2V4iMQXHGNJxeCfZXhSRBcQMfvkOWUdtfzlLgAENmZDcmo2TVmt8OSM2eXxBp3DjHSMFutqS7SbmemzBiR+xpKCNUIRkdkkYxhAkyGoBvyQFEJEefwSmmvBfJuJ6aKqKWnAkvGZOaZXTUgFqYULWNSHUckZuR1HIIimUExutRxwzOLROIG4vKmCKQt364mIlhSyzAf1m9lHZHJZrlAOMMztRRiKimp/rpdJDc9Awry5xTZCte7FHtuS8wJgeYGrex28xNTd086Dik7vUMscQOa8y4DoGtCCSkAKlNwpgNtphjrC6MIHUkR6YWxxs6Sc5xqn222mmCRFzIt8lEdKx+ikCtg91qS2WpwVfBelJCiQJwvzixfI9cxZQWgiSJelKnwBElKYtDOb2MFbhmUigbReQBV0Cg4+qMXSxXSyGUn4UbF8l+7qdSGnTC0XLCmahIgUHLhLOhpVCtw4CzYXvLQWQbJNmxoCsOKAxSgBJno75avolkRw8iIAFcsdc02e9iyCd8tHwmeSSoKTowIgvscSGZUOA7PuCN5b2BX9mQM7S0wYhMNU74zgsPBj3HU7wguAfnxxjFQGBE6pwN+GjME9zHY7zGp8wVxMShYX9NXvEWD3HbwJf4giO4CFIQxXScH1/TM+04kkBiAAAAAElFTkSuQmCC';

		(L.Icon.prototype as any)._createImg = (src: string, el: HTMLImageElement) => {
			el = el || document.createElement('img');
			if (src.includes('marker-icon.png')) {
				src = defaultMarker;
			}
			if (src.includes('marker-shadow.png')) {
				src = defaultMarkerShadow;
			}
			el.src = src;
			return el;
		};

		this.x = this.position.latitude
		this.y = this.position.longitude
		this.z = 18

		const latitude = document.getElementById('latitude') as HTMLInputElement
		const longitude = document.getElementById('longitude') as HTMLInputElement
		const viewSize = document.getElementById('viewSize') as HTMLInputElement

		latitude.setAttribute('value', this.x.toString())
		longitude.setAttribute('value', this.y.toString())
		viewSize.setAttribute('value', this.z.toString())
	}
}
</script>

<style scoped>
#map {
	width: 100%;
	height: 400px;
}
#mapComponent {
	width: 90%;
	display: grid;
	grid-template-columns: 80% 20%;
}

.transition, .gps button, .gps .property label, .gps .property input[type="text"] {
	-moz-transition: all 0.25s cubic-bezier(0.53, 0.01, 0.35, 1.5);
	-o-transition: all 0.25s cubic-bezier(0.53, 0.01, 0.35, 1.5);
	-webkit-transition: all 0.25s cubic-bezier(0.53, 0.01, 0.35, 1.5);
	transition: all 0.25s cubic-bezier(0.53, 0.01, 0.35, 1.5);
}

* {
	font-family: Helvetica, sans-serif;
	font-weight: light;
	-webkit-font-smoothing: antialiased;
}

.gps {
	position: relative;
	display: inline-block;
	width: 90%;
	box-sizing: border-box;
	padding: 30px 25px;
	background-color: white;
	border-radius: 40px;
	margin: 40px 0;
	left: 50%;
	-moz-transform: translate(-50%, 0);
	-ms-transform: translate(-50%, 0);
	-webkit-transform: translate(-50%, 0);
	transform: translate(-50%, 0);
	border: solid 4px black;
	background-color: #ABC;
}
h1 {
	color: #ff4a56;
	font-weight: 100;
	letter-spacing: 0.01em;
	margin-left: 15px;
	margin-bottom: 35px;
	text-transform: uppercase;
}
.property {
	position: relative;
	padding: 10px 0;
}
.property:first-of-type {
	padding-top: 0;
}
.property:last-of-type {
	padding-bottom: 0;
}
.property label {
	transform-origin: left center;
	font-weight: 100;
	letter-spacing: 0.01em;
	font-size: 22px;
	box-sizing: border-box;
	padding: 10px 15px;
	display: block;
	position: absolute;
	margin-top: -40px;
	z-index: 2;
	pointer-events: none;
}
.property input[type="text"] {
	appearance: none;
	background-color: none;
	line-height: 0;
	font-size: 17px;
	width: 100%;
	display: block;
	box-sizing: border-box;
	padding: 10px 15px;
	border-radius: 60px;
	font-weight: 100;
	letter-spacing: 0.01em;
	position: relative;
	z-index: 1;
}
.green {
	border: 1px solid green;
	color: green;
}
.red {
	border: 1px solid red;
	color: red;
}
.green:focus {
	background: green;
}
.red:focus {
	background: red;
}
.property input[type="text"]:focus {
	outline: none;
	color: white;
	margin-top: 30px;
}
.property input[type="text"]:valid {
	margin-top: 30px;
}
.property input[type="text"]:focus ~ label {
	-moz-transform: translate(0, -35px);
	-ms-transform: translate(0, -35px);
	-webkit-transform: translate(0, -35px);
	transform: translate(0, -35px);
}
.property input[type="text"]:valid ~ label {
	text-transform: uppercase;
	font-style: italic;
	-moz-transform: translate(5px, -35px) scale(0.6);
	-ms-transform: translate(5px, -35px) scale(0.6);
	-webkit-transform: translate(5px, -35px) scale(0.6);
	transform: translate(5px, -35px) scale(0.6);
}
</style>

