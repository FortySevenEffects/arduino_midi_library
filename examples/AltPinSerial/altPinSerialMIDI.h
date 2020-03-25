struct DefaultAltSerialSettings
{
    static const long BaudRate = 31250;
};

template <class SerialPort, class _Settings = DefaultAltSerialSettings>
class AltSerialMIDI
{
    typedef _Settings Settings;

    uint8_t rxPin = 0;
    uint8_t txPin = 0;

  friend class midi::MidiInterface<AltSerialMIDI<SerialPort>>;

public:
	AltSerialMIDI(SerialPort& inSerial, uint8_t inRxPin, uint8_t inTxPin)
        : mSerial(inSerial), rxPin(inRxPin), txPin(inTxPin)
	{
	};

protected:
	void begin()
	{
    mSerial.begin(Settings::BaudRate, SERIAL_8N1, rxPin, txPin);
	}

	bool beginTransmission(midi::MidiType)
	{
		return true;
	};

	void write(byte value)
	{
		mSerial.write(value);
	};

	void endTransmission()
	{
	};

	byte read()
	{
		return mSerial.read();
	};

	unsigned available()
	{
        return mSerial.available();
	};

private:
    SerialPort& mSerial;
};
