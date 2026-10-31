#include "DXLR03.h"

/* ---------- Constructor ---------- */
DXLR03::DXLR03(HardwareSerial& serial,
                       uint8_t rxPin,
                       uint8_t txPin,
                       uint32_t baud)
{
    _serial = &serial;
    _serial->begin(baud, SERIAL_8N1, rxPin, txPin);
}

/* ---------- Set Expected Frame ---------- */
void DXLR03::setExpect(const uint8_t *data, uint16_t len)
{
  if (len > EXPECT_BUF_SZ)
  {
    len = EXPECT_BUF_SZ;
  }
  memcpy(_expect, data, len);
  _expectLen = len;
}

/* ---------- Ring Buffer Basics ---------- */
void DXLR03::_rbWrite(uint8_t b)
{
  uint16_t next = (_rbHead + 1) % RB_SIZE;
  if (next != _rbTail) 
  { 
    _rb[_rbHead] = b;
    _rbHead = next; 
  }
}

void DXLR03::_rbClear()
{
  _rbTail = _rbHead = 0;
}

uint16_t DXLR03::_rbDataLen() const
{
  return (_rbHead >= _rbTail) ? (_rbHead - _rbTail)
                              : (RB_SIZE - _rbTail + _rbHead);
}

/* ---------- Expected Frame Matching ---------- */
bool DXLR03::_matchExpectInRb()
{
  if (_expectLen == 0) 
  {
    return true;          // Any frame mode
  }

  uint16_t len = _rbDataLen();
  if (len != _expectLen)
  {
    return false;
  }

  bool ok = true;
  for (uint16_t j = 0; j < _expectLen; ++j) 
  {
    if (_rb[(_rbTail + j) % RB_SIZE] != _expect[j]) 
    {
      ok = false; break;
    }
  }
  if (ok) 
  {
    return true;
  }

  return false;
}

/* ---------- State Transition ---------- */
void DXLR03::_changeState(State s)
{
  _state = s;
}

/* ---------- Start Reception ---------- */
void DXLR03::startReceive(uint32_t timeout)
{
  if(0 == timeout)
  {
    _ackDeadline = 0;
  }
  else
  {
    _ackDeadline = millis() + timeout;
  }
  _rbClear();                // Clear old data
  _changeState(ST_RX); // Enter waiting state
}

/* ---------- Transmission ---------- */
void DXLR03::send(const uint8_t *buf, uint16_t len)
{
  _serial->write(buf, len);
  if (_txDone)
  {
    _txDone();
  }
}

/* ---------- State Machine Polling ---------- */
void DXLR03::poll()
{
  /* 1. Receive data into ring buffer */
  while (_serial->available()) 
  {
    _rbWrite(_serial->read());
  }

  /* 2. State processing */
  switch (_state) 
  {
  case ST_IDLE:
    break;

  case ST_RX:
    /* 2.1 Expected frame received */
    if (_matchExpectInRb()) 
    {
      _changeState(ST_IDLE);

      uint8_t tmp[EXPECT_BUF_SZ];
      uint16_t n = (_expectLen > 0) ? _expectLen : _rbDataLen();
      if (n > EXPECT_BUF_SZ)
      {
        n = EXPECT_BUF_SZ;
      }

      /* Copy matched frame for callback */
      for (uint16_t i = 0; i < n; ++i) 
      {
        tmp[i] = _rb[(_rbTail + i) % RB_SIZE];
      }

      if (_rxDone) 
      {
        _rxDone(tmp, n);
      }
      _rbClear();
      
      break;
    }
    /* 2.2 Timeout */
    if(0 != _ackDeadline)
    {
      if (millis() >= _ackDeadline) 
      {
        _rbClear();
        _changeState(ST_RX_TIMEOUT);
      }
    }
    break;

  case ST_RX_TIMEOUT:
    _changeState(ST_IDLE);
    if (_rxTimeout) 
    {
      _rxTimeout();
    }
    break;
  }
}
