#define MESSAGE_GET 01
#define MESSAGE_CMD 02

#define CMD_DIGITAL 0x1e
#define CMD_ANALOG  0x1f
#define CMD_PWM     0x20
#define CMD_MOTOR   0x0a
#define CMD_PLAY    0x22

// longest message is 10 bytes long
#define MESSAGE_MAX_LEN 10

typedef struct _message {
	word header; // must be 0x55ff
	byte kind;
	byte len;
	union _payload {
		struct {
			byte portType;
			byte port;
			byte index;
		} get;
		byte cmd;
		struct _write {
			byte cmd;
			byte port;
			float value;
		} write;
		struct _motor {
			byte cmd;
			byte port;
			float speed;
		} motor;
		struct _play {
			byte cmd;
			byte port;
			word frequency; // unit ?
			word duration;  // ms
		} play;
	} payload;
} message;

typedef struct _resultMessage {
	word header; // must be 0x55ff
	byte index;
	float value;
	char suffix[2]; // \r \n
} resultMessage;
