#ifndef COM_UTIL_H
#define COM_UTIL_H

bool open_com_port( HANDLE &hCOM_HANDLE, int i_com_port, int i_baud_rate);
bool set_com_port( HANDLE &hCOM_HANDLE, int i_baud_rate);
bool close_com_port( HANDLE &hCOM_HANDLE);

class com_sentry
{
    public:
  	    com_sentry(HANDLE &com, unsigned int com_port): m_hCOM(com)
        {
		    open_com_port(m_hCOM, com_port, 115200);
	    }


  	    ~com_sentry()
        {
 		    close_com_port(m_hCOM);
        }

    private:
  	    HANDLE &m_hCOM;
};


#endif

