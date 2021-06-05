export enum VideoStreamType {
	VIDEO_STREAM_TYPE_RTSP = 0, // Stream is RTSP
	VIDEO_STREAM_TYPE_RTPUDP = 1, // Stream is RTP UDP (URI gives the port number)
	VIDEO_STREAM_TYPE_TCP_MPEG = 2, // Stream is MPEG on TCP
	VIDEO_STREAM_TYPE_MPEG_TS_H264 = 3, // Stream is h.264 on MPEG TS (URI gives the port number)
	VIDEO_STREAM_TYPE_ENUM_END = 4, // 
}