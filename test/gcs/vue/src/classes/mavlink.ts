
import {MAVLinkModule, MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';  
import {messageRegistry} from '../classes/mavlink/message-registry';
import { Heartbeat } from '../classes/mavlink/messages/heartbeat';

const mavLink = new MAVLinkModule(messageRegistry);

export { mavLink }
