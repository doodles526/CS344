#Josh Deare
#dearej@onid.orst.edu
#CS344-400
#HW6

import socket
import threading
import signal
import sys
import argparse
from constants import ClientType, XMLHier

manage = None

def sigint_handler(signal, frame):
    global manage
    manage.stop()
    sys.exit(0)

class Manager:
    def __init__(self, host='', port=32400, max_scan=3000000):
        self.host = host
        self.port = port
        self.max_scan = max_scan
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.temp_conn = None
        self.threads = list()
        self.perfect_nums = list()

        self._current_max = 0
        self.max_lock = threading.Lock()
        self._init_socket()

    def stop(self):
        self._cleanup()

    def _cleanup(self):
        """
        Cleanup all Manager's resources
        """
        for thread in self.threads:
            thread.stop()
        
        try:
            self.temp_conn[0].close()
        except:
            pass

        try:
            self.sock.close()
        except:
            pass
        
        for thread in self.threads:
            conn.join()

    def _init_socket(self):
        self.sock.bind((self.host, self.port))
        self.sock.listen(5)
    
    def read_xml(self, conn):
        """
        Reads an xml payload and returns a XMLHier
        of the results
        """
        pass
    
    def _client_type(self, conn):
        """
        Returns the type of the client.
        """
        return self.read_xml(conn).client-type.value

    def run(self):
        while True:
            # We trash the address since we never need it
            # for this particular application
            self.temp_conn = self.sock.accept()
            my_type = self._client_type(self.temp_conn)

            if my_type == ClientType.REPORT:
                self.threads.append(ReportClient(self.temp_conn, self))
            else:
                self.threads.append(ComputeClient(self.temp_conn, self, 15))
            
            self.threads[-1].start()
        
class Client(threading.Thread):
    """
    Superclass for other clients
    """
    def __init__(self, conn, addr, manager):
        self.conn = conn
        self.addr = addr
        self.manager = manager
        self._stop = threading.Event()

    def stop(self):
        """
        Allows a public facing way to cleanly exit
        """
        self._stop.set()

    def _cleanup(self):
        """
        Private method to cleanup before exiting
        """
        close(self.conn)

    def read_xml(self):
        """
        Returns an XMLHier object with the parsed xml

        Note: this holds until it gets a value
        """
        pass

    def run():
        """
        Main function called by starting our thread
        """
        self._cleanup()


class ReportClient(Client):
    """
    Subclass for a reporter client
    """
    def __init__(self, conn, addr, manager):
        super(ReportClient, self).__init__(conn, addr, manager)


class ComputeClient(Client):
    """
    Subclass for a compute client
    """
    def __init__(self, conn, addr, manager, seconds):
        super(ComputeClient, self).__init__(conn, addr, manager)
        self.seconds = seconds

    def append_perfect_nums(self, perfect):
        """
        Add perfect number to the found list
        """
        self.manager.perfect_nums.append(perfect)

    def send_range(self, mps):
        """
        Calculate the range for the compute to carry out
        """
        self.manager.max_lock.acquire()
        max = self.manager._current_max
        lower = max
        sum = 0

        max_mods = mps * 15
        
        while sum < max_mods:
            sum = sum + max
            max = max+1

        xml = self.marshall_range(lower, max)
        self.conn.write(xml)

    def marshall_range(self, lower, max):
        """
        Returns valid xml for range by using the XMLHier class
        """
        # TODO: Implement
        pass

    def process_xml(self):
        """
        Reads in XML and processes it
        """
        xml = self.read_xml()
        if hasattr(xml, mods-per-second):
            self.send_range(xml.mods-per-second.value)
        elif hasattr(xml, perfect-number):
            self.append_perfect_nums(xml.perfect-number.value)
        elif hasattr(xml, status):
            self.handle_status(xml.status.value)
        else:
            self.stop()

    def run():
        # Get mods per second
        self.process_xml()
        while not self._stop.isSet():
            self.process_xml()
        self._cleanup()


def main():
    signal.signal(signal.SIGINT, sigint_handler)

    parser = argparse.ArgumentParser(description="Perfect Number Manager")
    parser.add_argument("-p", "--port", type=int, default=32400,
                        help="Passive Bind Port")
    parser.add_argument("-m", "--max_scanval", type=int, default=3000000,
                        help="Max Perfect Number to find")
    args = parser.parse_args()

    global manage
    manage = Manager('', args.port, args.max_scanval)
    manage.run()

if __name__ == "__main__":
    main()
