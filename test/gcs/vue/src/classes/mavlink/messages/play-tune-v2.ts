import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {TuneFormat} from '../enums/tune-format';
/*
Play vehicle tone/tune (buzzer). Supersedes message PLAY_TUNE.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// format Tune format uint32_t
// tune Tune definition as a NULL-terminated string. char
export class PlayTuneV2 extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public format!: TuneFormat;
	public tune!: string;
	public _message_id: number = 400;
	public _message_name: string = 'PLAY_TUNE_V2';
	public _crc_extra: number = 110;
	public _message_fields: [string, string, boolean][] = [
		['format', 'uint32_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['tune', 'char', false],
	];
}