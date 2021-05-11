
const path = require('path')
const VueLoaderPlugin = require('vue-loader/lib/plugin')

module.exports = {
	entry: './src/main.ts',
	module: {
	rules: [
		{
			test: /\.vue$/,
			loader: 'vue-loader'
		},
		/*
		{
			test: /\.ts?$/,
			use: 'ts-loader',
			exclude: /node_modules/,
		},
		*/
	],
	},
	resolve: {
	extensions: ['.tsx', '.ts', '.js'],
	},
	output: {
	filename: 'bundle.js',
	path: path.resolve(__dirname, 'dist'),
	},
	plugins: [
		new VueLoaderPlugin()
	]
};
