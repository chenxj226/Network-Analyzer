# Network-Analyzer

## Interface and Usage Instructions

The program will prompt you to select a network card the first time you open it. If the desired network card is not listed, click the "Refresh" button on the right. Once you find the network card, you can proceed with sending or receiving packets.

### 1. Receiving Packets

- Click "Start" to begin receiving packets, then click "Stop" to stop receiving. 
- The log below will display the information. If no packets are received, the "View" and "Save" buttons will be disabled.
- If packets are received, you can click "View" to examine them and "Save" to store them.
- The "View" interface will allow you to select a packet and click "Show" to display it. Right-clicking offers various display options.
- When starting reception again, you will be prompted whether to clear the data. Click "Yes" to receive packets again.

### 2. Sending Packets

- Click "Send Setting" to configure the sending mode. There are two modes:
  - **Continue**: Sends packets continuously based on the send packet rate.
  - **Burst**: Sends a fixed number of packets based on the send packet rate.
- If "Start" is clicked without configuration, the default mode is "Continue" at 1 packet/s.

### Receiving Features

The receiving function is comprehensive. However, the sending function lacks some features:
1. The content of the packets to be sent is set in the user interface under the "New" button.
2. When sending, the source address, destination address, and type should be displayed.
3. Simultaneous receiving while sending can be added to the receiving window.

## Code Explanation

- **widget.cpp**: Initializes the setting window contents and connects slot functions. The implementation code for each button can be found based on their functions and slot functions.
- **netpcap.cpp**: Defines classes that include finding network cards and using WinPcap for packet sending and receiving.
- **qhexmonitor.cpp**: Defines the view window.
- **recvpacketthread.cpp**: Creates the packet receiving thread.
- **recvviewwindow.cpp**: Sets up the view window and data transfer.
- **sendpacketthread.cpp**: Creates the packet sending thread.
- **sendsettingwindow.cpp**: Defines the send setting window for packet sending.

Due to the Windows timer resolution being limited to milliseconds, the sending function uses a countdown method. Every 10ms, a countdown occurs. Based on the send rate, e.g., for 1000 packets/s, 100 packets are sent, then it stops and waits for 10ms, and then another 100 packets are sent, repeating continuously.

