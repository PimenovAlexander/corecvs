import { MAVLinkMessage } from "@ifrunistuttgart/node-mavlink";
import * as $ from "jquery";
import { mavLink } from '../classes/mavlink';

export function send(url: string, message: MAVLinkMessage, received: ((data: any) => void) | null = null) {
	$.ajax({
		url: `${window.location.href}${url}`,
		data: mavLink.pack([message]).toString('hex'),
		success: (received != null) ? data => { received(data) } : _ => { 1 + 1; }
	})
}
