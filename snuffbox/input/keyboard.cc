#include "../../snuffbox/input/keyboard.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/js/js_wrapper.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------
	namespace environment {
		namespace
		{
			Keyboard* globalInstance = nullptr;
		}

		bool has_keyboard() { return globalInstance != nullptr; }

		Keyboard& keyboard()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//--------------------------------------------------------------------------------------
	Keyboard::Keyboard()
	{
		for (unsigned int i = 0; i < 255; ++i)
		{
			key_states_[i].pressed = false;
			key_states_[i].down = false;
			key_states_[i].released = false;
		}

		environment::globalInstance = this;
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::ResetStates()
	{
		for (unsigned int i = 0; i < 255; ++i)
		{
			key_states_[i].pressed = false;
			key_states_[i].released = false;
		}
	}

	//--------------------------------------------------------------------------------------
	bool Keyboard::IsPressed(Key key)
	{
		return key_states_[key].pressed;
	}

	//--------------------------------------------------------------------------------------
	bool Keyboard::IsDown(Key key)
	{
		return key_states_[key].down;
	}

	//--------------------------------------------------------------------------------------
	bool Keyboard::IsReleased(Key key)
	{
		return key_states_[key].released;
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::ReceiveEvent(KeyData evt)
	{
		queue_.push(evt);
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::Update()
	{
		ResetStates();

		while (!queue_.empty())
		{
			const KeyData& evt = queue_.front();

			switch (evt.type)
			{
			case KeyboardEnums::KeyEvent::kPressed:
				if (!key_states_[evt.key].down)
					key_states_[evt.key].pressed = true;
				
				key_states_[evt.key].down = true;
				last_pressed_ = evt.key;
				break;

			case KeyboardEnums::KeyEvent::kReleased:
				key_states_[evt.key].down = false;
				key_states_[evt.key].released = true;
				last_released_ = evt.key;
				break;
			}

			queue_.pop();
		}
	}

	//--------------------------------------------------------------------------------------
	std::string Keyboard::GetLastPressed()
	{
		return KeyToString(last_pressed_);
	}

	//--------------------------------------------------------------------------------------
	std::string Keyboard::GetLastReleased()
	{
		return KeyToString(last_released_);
	}

	//--------------------------------------------------------------------------------------
	Keyboard::~Keyboard()
	{

	}

	//--------------------------------------------------------------------------------------
	void Keyboard::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] =
		{
			JSFunctionRegister("isPressed", JSIsPressed),
			JSFunctionRegister("isDown", JSIsDown),
			JSFunctionRegister("isReleased", JSIsReleased),
			JSFunctionRegister("lastPressed", JSLastPressed),
			JSFunctionRegister("lastReleased", JSLastReleased)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::JSIsPressed(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("S");

		Key key = StringToKey(wrapper.GetString(0).c_str());

		bool check = environment::keyboard().IsPressed(key);
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::JSIsDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("S");

		Key key = StringToKey(wrapper.GetString(0).c_str());

		bool check = environment::keyboard().IsDown(key);
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::JSIsReleased(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("S");

		Key key = StringToKey(wrapper.GetString(0).c_str());

		bool check = environment::keyboard().IsReleased(key);
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::JSLastPressed(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("V");

		Keyboard& keyboard = environment::keyboard();

		wrapper.ReturnString(keyboard.GetLastPressed().c_str());
	}

	//--------------------------------------------------------------------------------------
	void Keyboard::JSLastReleased(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("V");

		Keyboard& keyboard = environment::keyboard();

		wrapper.ReturnString(keyboard.GetLastReleased().c_str());
	}

	//--------------------------------------------------------------------------------------
	Key StringToKey(const char* name)
	{
		if (strcmp(name, "Backspace") == 0)			return Key::kBackspace;
		if (strcmp(name, "Tab") == 0)						return Key::kTab;
		if (strcmp(name, "Clear") == 0)					return Key::kClear;
		if (strcmp(name, "Enter") == 0)					return Key::kEnter;
		if (strcmp(name, "Shift") == 0)					return Key::kShift;
		if (strcmp(name, "Control") == 0)				return Key::kControl;
		if (strcmp(name, "Alt") == 0)						return Key::kAlt;
		if (strcmp(name, "Pause") == 0)					return Key::kPause;
		if (strcmp(name, "Caps Lock") == 0)			return Key::kCapsLock;
		if (strcmp(name, "Escape") == 0)				return Key::kEscape;
		if (strcmp(name, "Space") == 0)					return Key::kSpace;
		if (strcmp(name, "Page Up") == 0)				return Key::kPageUp;
		if (strcmp(name, "Page Down") == 0)			return Key::kPageUp;
		if (strcmp(name, "End") == 0)						return Key::kEnd;
		if (strcmp(name, "Home") == 0)					return Key::kHome;
		if (strcmp(name, "Left") == 0)					return Key::kLeft;
		if (strcmp(name, "Up") == 0)						return Key::kUp;
		if (strcmp(name, "Right") == 0)					return Key::kRight;
		if (strcmp(name, "Down") == 0)					return Key::kDown;
		if (strcmp(name, "Select") == 0)				return Key::kSelect;
		if (strcmp(name, "Print") == 0)					return Key::kPrint;
		if (strcmp(name, "Execute") == 0)				return Key::kExecute;
		if (strcmp(name, "Print Screen") == 0)	return Key::kPrintScreen;
		if (strcmp(name, "Insert") == 0)				return Key::kInsert;
		if (strcmp(name, "Delete") == 0)				return Key::kDelete;
		if (strcmp(name, "Help") == 0)					return Key::kHelp;
		if (strcmp(name, "0") == 0)					    return Key::kZero;
		if (strcmp(name, "1") == 0)						  return Key::kOne;
		if (strcmp(name, "2") == 0)						  return Key::kTwo;
		if (strcmp(name, "3") == 0)					    return Key::kThree;
		if (strcmp(name, "4") == 0)					    return Key::kFour;
		if (strcmp(name, "5") == 0)					    return Key::kFive;
		if (strcmp(name, "6") == 0)						  return Key::kSix;
		if (strcmp(name, "7") == 0)					    return Key::kSeven;
		if (strcmp(name, "8") == 0)					    return Key::kEight;
		if (strcmp(name, "9") == 0)					    return Key::kNine;
		if (strcmp(name, "A") == 0)							return Key::kA;
		if (strcmp(name, "B") == 0)							return Key::kB;
		if (strcmp(name, "C") == 0)							return Key::kC;
		if (strcmp(name, "D") == 0)							return Key::kD;
		if (strcmp(name, "E") == 0)							return Key::kE;
		if (strcmp(name, "F") == 0)							return Key::kF;
		if (strcmp(name, "G") == 0)							return Key::kG;
		if (strcmp(name, "H") == 0)							return Key::kH;
		if (strcmp(name, "I") == 0)							return Key::kI;
		if (strcmp(name, "J") == 0)							return Key::kJ;
		if (strcmp(name, "K") == 0)							return Key::kK;
		if (strcmp(name, "L") == 0)							return Key::kL;
		if (strcmp(name, "M") == 0)							return Key::kM;
		if (strcmp(name, "N") == 0)							return Key::kN;
		if (strcmp(name, "O") == 0)							return Key::kO;
		if (strcmp(name, "P") == 0)							return Key::kP;
		if (strcmp(name, "Q") == 0)							return Key::kQ;
		if (strcmp(name, "R") == 0)							return Key::kR;
		if (strcmp(name, "S") == 0)							return Key::kS;
		if (strcmp(name, "T") == 0)							return Key::kT;
		if (strcmp(name, "U") == 0)							return Key::kU;
		if (strcmp(name, "V") == 0)							return Key::kV;
		if (strcmp(name, "W") == 0)							return Key::kW;
		if (strcmp(name, "X") == 0)							return Key::kX;
		if (strcmp(name, "Y") == 0)							return Key::kY;
		if (strcmp(name, "Z") == 0)							return Key::kZ;
		if (strcmp(name, "Left Windows") == 0)	return Key::kLeftWindowsKey;
		if (strcmp(name, "Right Windows") == 0)	return Key::kRightWindowsKey;
		if (strcmp(name, "Applications") == 0)	return Key::kApplicationsKey;
		if (strcmp(name, "Sleep") == 0)					return Key::kSleep;
		if (strcmp(name, "Numpad0") == 0)				return Key::kNumPad0;
		if (strcmp(name, "Numpad1") == 0)				return Key::kNumPad1;
		if (strcmp(name, "Numpad2") == 0)				return Key::kNumPad2;
		if (strcmp(name, "Numpad3") == 0)				return Key::kNumPad3;
		if (strcmp(name, "Numpad4") == 0)				return Key::kNumPad4;
		if (strcmp(name, "Numpad5") == 0)				return Key::kNumPad5;
		if (strcmp(name, "Numpad6") == 0)				return Key::kNumPad6;
		if (strcmp(name, "Numpad7") == 0)				return Key::kNumPad7;
		if (strcmp(name, "Numpad8") == 0)				return Key::kNumPad8;
		if (strcmp(name, "Numpad9") == 0)				return Key::kNumPad9;
		if (strcmp(name, "Multiply") == 0)			return Key::kMultiply;
		if (strcmp(name, "Plus") == 0)					return Key::kPlus;
		if (strcmp(name, "Seperator") == 0)			return Key::kSeperator;
		if (strcmp(name, "Minus") == 0)					return Key::kMinus;
		if (strcmp(name, "Decimal") == 0)				return Key::kDecimal;
		if (strcmp(name, "Divide") == 0)				return Key::kDivide;
		if (strcmp(name, "F1") == 0)						return Key::kF1;
		if (strcmp(name, "F2") == 0)						return Key::kF2;
		if (strcmp(name, "F3") == 0)						return Key::kF3;
		if (strcmp(name, "F4") == 0)						return Key::kF4;
		if (strcmp(name, "F5") == 0)						return Key::kF5;
		if (strcmp(name, "F6") == 0)						return Key::kF6;
		if (strcmp(name, "F7") == 0)						return Key::kF7;
		if (strcmp(name, "F8") == 0)						return Key::kF8;
		if (strcmp(name, "F9") == 0)						return Key::kF9;
		if (strcmp(name, "F10") == 0)						return Key::kF10;
		if (strcmp(name, "F11") == 0)						return Key::kF11;
		if (strcmp(name, "F12") == 0)						return Key::kF12;
		if (strcmp(name, "F13") == 0)						return Key::kF13;
		if (strcmp(name, "F14") == 0)						return Key::kF14;
		if (strcmp(name, "F15") == 0)						return Key::kF15;
		if (strcmp(name, "F16") == 0)						return Key::kF16;
		if (strcmp(name, "F17") == 0)						return Key::kF17;
		if (strcmp(name, "F18") == 0)						return Key::kF18;
		if (strcmp(name, "F19") == 0)						return Key::kF19;
		if (strcmp(name, "F20") == 0)						return Key::kF20;
		if (strcmp(name, "F21") == 0)						return Key::kF21;
		if (strcmp(name, "F22") == 0)						return Key::kF22;
		if (strcmp(name, "F23") == 0)						return Key::kF23;
		if (strcmp(name, "F24") == 0)						return Key::kF24;
		if (strcmp(name, "Numlock") == 0)				return Key::kNumlock;
		if (strcmp(name, "Scroll Lock") == 0)		return Key::kScrollLock;
		if (strcmp(name, "Left Shift") == 0)		return Key::kLeftShift;
		if (strcmp(name, "Right Shift") == 0)		return Key::kRightShift;
		if (strcmp(name, "Left Control") == 0)	return Key::kLeftControl;
		if (strcmp(name, "Right Control") == 0)	return Key::kRightControl;
		if (strcmp(name, "Left Menu") == 0)			return Key::kLeftMenu;
		if (strcmp(name, "Right Menu") == 0)		return Key::kRightMenu;
		if (strcmp(name, "Browse Back") == 0)		return Key::kBrowserBack;
		if (strcmp(name, "Browse Forward") == 0)return Key::kBrowserForward;
		if (strcmp(name, "Browse Refresh") == 0)return Key::kBrowserRefresh;
		if (strcmp(name, "Browse Stop") == 0)		return Key::kBrowserStop;
		if (strcmp(name, "Browse Search") == 0)	return Key::kBrowserSearch;
		if (strcmp(name, "Browse Fave") == 0)		return Key::kBrowserFavorites;
		if (strcmp(name, "Browse Home") == 0)		return Key::kBrowserHome;
		if (strcmp(name, "Volume Mute") == 0)		return Key::kVolumeMute;
		if (strcmp(name, "Volume Down") == 0)		return Key::kVolumeDown;
		if (strcmp(name, "Volume Up") == 0)			return Key::kVolumeUp;
		if (strcmp(name, "Next Track") == 0)		return Key::kNextTrack;
		if (strcmp(name, "Previous Track") == 0)return Key::kPreviousTrack;
		if (strcmp(name, "Stop Media") == 0)		return Key::kStopMedia;
		if (strcmp(name, "Play Pause") == 0)		return Key::kPlayPause;
		if (strcmp(name, "Mail") == 0)					return Key::kLaunchMail;
		if (strcmp(name, "Select Media") == 0)	return Key::kSelectMedia;
		if (strcmp(name, "Launch App 1") == 0)	return Key::kLaunchApp1;
		if (strcmp(name, "Launch App 2") == 0)	return Key::kLaunchApp2;
		if (strcmp(name, "OEM1") == 0)					return Key::kOEM1;
		if (strcmp(name, "OEM Plus") == 0)			return Key::kOEMPlus;
		if (strcmp(name, "OEM Comma") == 0)			return Key::kOEMComma;
		if (strcmp(name, "OEM Minus") == 0)			return Key::kOEMMinus;
		if (strcmp(name, "OEM Period") == 0)		return Key::kOEMPeriod;
		if (strcmp(name, "OEM2") == 0)					return Key::kOEM2;
		if (strcmp(name, "OEM3") == 0)					return Key::kOEM3;
		if (strcmp(name, "OEM4") == 0)					return Key::kOEM4;
		if (strcmp(name, "OEM5") == 0)					return Key::kOEM5;
		if (strcmp(name, "OEM6") == 0)					return Key::kOEM6;
		if (strcmp(name, "OEM7") == 0)					return Key::kOEM7;
		if (strcmp(name, "OEM8") == 0)					return Key::kOEM8;
		if (strcmp(name, "OEM102") == 0)				return Key::kOEM102;
		if (strcmp(name, "Process") == 0)				return Key::kProcess;
		if (strcmp(name, "Packet") == 0)				return Key::kPacket;
		if (strcmp(name, "Attn") == 0)					return Key::kAttn;
		if (strcmp(name, "CrSel") == 0)					return Key::kCrSel;
		if (strcmp(name, "ExSel") == 0)					return Key::kExSel;
		if (strcmp(name, "Erase EOF") == 0)			return Key::kEraseEOF;
		if (strcmp(name, "Play") == 0)					return Key::kPlay;
		if (strcmp(name, "Zoom") == 0)					return Key::kZoom;
		if (strcmp(name, "PA1") == 0)						return Key::kPA1;
		if (strcmp(name, "OEM Clear") == 0)			return Key::kOEMClear;

		return Key::kBackspace;
	}

	//--------------------------------------------------------------------------------------
	std::string KeyToString(Key key)
	{
		switch (key)
		{
		case Key::kBackspace:						return "Backspace";
		case Key::kTab:									return "Tab";
		case Key::kClear:								return "Clear";
		case Key::kEnter:								return "Enter";
		case Key::kShift:								return "Shift";
		case Key::kControl:							return "Control";
		case Key::kAlt:									return "Alt";
		case Key::kPause:								return "Pause";
		case Key::kCapsLock:						return "Caps Lock";
		case Key::kEscape:							return "Escape";
		case Key::kSpace:								return "Space";
		case Key::kPageUp:							return "Page Up";
		case Key::kEnd:									return "End";
		case Key::kHome:								return "Home";
		case Key::kLeft:								return "Left";
		case Key::kUp:									return "Up";
		case Key::kRight:								return "Right";
		case Key::kDown:								return "Down";
		case Key::kSelect:							return "Select";
		case Key::kPrint:								return "Print";
		case Key::kExecute:							return "Execute";
		case Key::kPrintScreen:					return "Print Screen";
		case Key::kInsert:							return "Insert";
		case Key::kDelete:							return "Delete";
		case Key::kHelp:								return "Help";
		case Key::kZero:								return "0";
		case Key::kOne:									return "1";
		case Key::kTwo:									return "2";
		case Key::kThree:								return "3";
		case Key::kFour:								return "4";
		case Key::kFive:								return "5";
		case Key::kSix:									return "6";
		case Key::kSeven:								return "7";
		case Key::kEight:								return "8";
		case Key::kNine:								return "9";
		case Key::kA:										return "A";
		case Key::kB:										return "B";
		case Key::kC:										return "C";
		case Key::kD:										return "D";
		case Key::kE:										return "E";
		case Key::kF:										return "F";
		case Key::kG:										return "G";
		case Key::kH:										return "H";
		case Key::kI:										return "I";
		case Key::kJ:										return "J";
		case Key::kK:										return "K";
		case Key::kL:										return "L";
		case Key::kM:										return "M";
		case Key::kN:										return "N";
		case Key::kO:										return "O";
		case Key::kP:										return "P";
		case Key::kQ:										return "Q";
		case Key::kR:										return "R";
		case Key::kS:										return "S";
		case Key::kT:										return "T";
		case Key::kU:										return "U";
		case Key::kV:										return "V";
		case Key::kW:										return "W";
		case Key::kX:										return "X";
		case Key::kY:										return "Y";
		case Key::kZ:										return "Z";
		case Key::kLeftWindowsKey:			return "Left Windows";
		case Key::kRightWindowsKey:			return "Right Windows";
		case Key::kApplicationsKey:			return "Applications";
		case Key::kSleep:								return "Sleep";
		case Key::kNumPad0:							return "Numpad0";
		case Key::kNumPad1:							return "Numpad1";
		case Key::kNumPad2:							return "Numpad2";
		case Key::kNumPad3:							return "Numpad3";
		case Key::kNumPad4:							return "Numpad4";
		case Key::kNumPad5:							return "Numpad5";
		case Key::kNumPad6:							return "Numpad6";
		case Key::kNumPad7:							return "Numpad7";
		case Key::kNumPad8:							return "Numpad8";
		case Key::kNumPad9:							return "Numpad9";
		case Key::kMultiply:						return "Multiply";
		case Key::kPlus:								return "Plus";
		case Key::kSeperator:						return "Seperator";
		case Key::kMinus:								return "Minus";
		case Key::kDecimal:							return "Decimal";
		case Key::kDivide:							return "Divide";
		case Key::kF1:									return "F1";
		case Key::kF2:									return "F2";
		case Key::kF3:									return "F3";
		case Key::kF4:									return "F4";
		case Key::kF5:									return "F5";
		case Key::kF6:									return "F6";
		case Key::kF7:									return "F7";
		case Key::kF8:									return "F8";
		case Key::kF9:									return "F9";
		case Key::kF10:									return "F10";
		case Key::kF11:									return "F11";
		case Key::kF12:									return "F12";
		case Key::kF13:									return "F13";
		case Key::kF14:									return "F14";
		case Key::kF15:									return "F15";
		case Key::kF16:									return "F16";
		case Key::kF17:									return "F17";
		case Key::kF18:									return "F18";
		case Key::kF19:									return "F19";
		case Key::kF20:									return "F20";
		case Key::kF21:									return "F21";
		case Key::kF22:									return "F22";
		case Key::kF23:									return "F23";
		case Key::kF24:									return "F24";
		case Key::kNumlock:							return "Numlock";
		case Key::kScrollLock:					return "Scroll Lock";
		case Key::kLeftShift:						return "Left Shift";
		case Key::kRightShift:					return "Right Shift";
		case Key::kLeftControl:					return "Left Control";
		case Key::kRightControl:				return "Right Control";
		case Key::kLeftMenu:						return "Left Menu";
		case Key::kRightMenu:						return "Right Menu";
		case Key::kBrowserBack:					return "Browser Back";
		case Key::kBrowserForward:			return "Browser Forward";
		case Key::kBrowserRefresh:			return "Browser Refresh";
		case Key::kBrowserStop:					return "Browser Stop";
		case Key::kBrowserSearch:				return "Browser Search";
		case Key::kBrowserFavorites:		return "Browser Fave";
		case Key::kBrowserHome:					return "Browser Home";
		case Key::kVolumeMute:					return "Volume Mute";
		case Key::kVolumeDown:					return "Volume Down";
		case Key::kVolumeUp:						return "Volume Up";
		case Key::kNextTrack:						return "Next Track";
		case Key::kPreviousTrack:				return "Previous Track";
		case Key::kStopMedia:						return "Stop Media";
		case Key::kPlayPause:						return "Play Pause";
		case Key::kLaunchMail:					return "Mail";
		case Key::kSelectMedia:					return "Select Media";
		case Key::kLaunchApp1:					return "Launch App 1";
		case Key::kLaunchApp2:					return "Launch App 2";
		case Key::kOEM1:								return "OEM1";
		case Key::kOEMPlus:							return "OEM Plus";
		case Key::kOEMComma:						return "OEM Comma";
		case Key::kOEMMinus:						return "OEM Minus";
		case Key::kOEMPeriod:						return "OEM Period";
		case Key::kOEM2:								return "OEM2";
		case Key::kOEM3:								return "OEM3";
		case Key::kOEM4:								return "OEM4";
		case Key::kOEM5:								return "OEM5";
		case Key::kOEM6:								return "OEM6";
		case Key::kOEM7:								return "OEM7";
		case Key::kOEM8:								return "OEM8";
		case Key::kOEM102:							return "OEM102";
		case Key::kProcess:							return "Process";
		case Key::kPacket:							return "Packet";
		case Key::kAttn:								return "Attn";
		case Key::kCrSel:								return "CrSel";
		case Key::kExSel:								return "ExSel";
		case Key::kEraseEOF:						return "Erase EOF";
		case Key::kPlay:								return "Play";
		case Key::kZoom:								return "Zoom";
		case Key::kPA1:									return "PA1";
		case Key::kOEMClear:						return "OEM Clear";
		default:												return "None";
		}
	}

	// *dead* I pity the creator of the enum more than myself. Jesus.
}