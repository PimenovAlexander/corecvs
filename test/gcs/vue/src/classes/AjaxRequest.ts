import { MAVLinkMessage } from "@ifrunistuttgart/node-mavlink";
import * as $ from "jquery";
import { mavLink } from '../classes/mavlink';

export function send(url: string, message: MAVLinkMessage, log = false) {
	if (log) {
		console.log(`AJAX_REQUEST: { url = ${window.location.href}${url} }`)
	}
	$.ajax({
		url: `${window.location.href}${url}`,
		data: mavLink.pack([message]).toString(),
		success: (log) ? console.log : _ => { const item = 0; }
	})
}
