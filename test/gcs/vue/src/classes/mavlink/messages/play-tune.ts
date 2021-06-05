import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Control vehicle tone generation (buzzer).
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// tune tune in board specific format char
// tune2 tune extension (appended to tune) char
export class PlayTune extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public tune!: string;
	public tune2!: string;
	public _message_id: number = 258;
	public _message_name: string = 'PLAY_TUNE';
	public _crc_extra: number = 187;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['tune', 'char', false],
		['tune2', 'char', true],
	];
}