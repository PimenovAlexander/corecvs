
<template>
	<ul class="menu">
		<li id="mapButton" @click="select(0)">Map</li>
		<li id="cameraButton" @click="select(1)">Camera</li>
	</ul>
</template>

<script lang="ts">
import { Component, Prop, Vue } from 'vue-property-decorator';

// A component that displays buttons which switch elements displayed in the web application

@Component
export default class Menu extends Vue {
	activeWindow = 0;

	@Prop() private changeActiveWindow!: (windowIndex: number) => void
	@Prop() private windows!: number[];

	mounted() {
		this.select(this.activeWindow)
	}

	// A method that changes "activeWindow" property and menu button's styles to show which option is currently selected
	select(menuOption: number) {
		if (menuOption < 0) throw Error(`Menu option is ${menuOption} < 0`)
		if (menuOption >= this.windows.length) throw Error(`Menu option is ${menuOption} >= numberOfMenuOptions`)
		this.activeWindow = menuOption;
		this.changeActiveWindow(menuOption)

		this.windows.forEach((window, index) => {
			const button = document.getElementById(`${window}Button`)
			if (button == undefined) throw Error(`Error: No menu element "${window}" found`)
			button.classList.remove(...button.classList)
			button.classList.add(index == menuOption ? "selected" : "notSelected")
			button.classList.add('noselect')
		})
	}
}
</script>

<style scoped>
ul {
	padding: 0;
	list-style-type: none;

	display: grid;
	grid-template-rows: repeat(2, 140px);
}

li {
	width: 140px;
	height: 100px;
	font-size: 20px;
	text-align: center;
	line-height: 7rem;
	font-family: sans-serif;
	text-transform: uppercase;
	letter-spacing: 1px;
	position: relative;
	transition: 0.3s;
	margin: 3rem;
}

.notSelected::before,
.notSelected::after {
	content: '';
	position: absolute;
	width: inherit;
	height: inherit;
	top: 0;
	left: 0;
	transition: 0.3s;
}

.notSelected::before {
	background-color: white;
	z-index: -1;
	box-shadow: 0.2rem 0.2rem 0.5rem rgba(0, 0, 0, 0.2);
}

.notSelected::after {
	background-color: goldenrod;
	transform: translate(1.5rem, 1.5rem);
	z-index: -2;
}

.notSelected:hover {
	transform: translate(1.5rem, 1.5rem);
	color: white;
}

.notSelected:hover::before {
	background-color: goldenrod;
}

.notSelected:hover::after {
	background-color: white;
	transform: translate(-1.5rem, -1.5rem);
}

/* Active */

.selected {
	padding-left: 1rem;
	padding-top: 1rem;
	color: white;
}

.selected::before,
.selected::after {
	content: '';
	position: absolute;
	width: inherit;
	height: inherit;
	top: 1.5rem;
	left: 1.5rem;
	transition: 0.3s;
}

.selected::before {
	background-color: white;
	transform: translate(-1.5rem, -1.5rem);
	z-index: -2;
}

.selected::after {
	background-color: #282;
	z-index: -1;
	box-shadow: -0.2rem -0.2rem -0.5rem rgba(0, 0, 0, 0.2);
}

.noselect {
  -webkit-touch-callout: none; /* iOS Safari */
    -webkit-user-select: none; /* Safari */
     -khtml-user-select: none; /* Konqueror HTML */
       -moz-user-select: none; /* Old versions of Firefox */
        -ms-user-select: none; /* Internet Explorer/Edge */
            user-select: none; /* Non-prefixed version, currently
                                  supported by Chrome, Edge, Opera and Firefox */
}

</style>
