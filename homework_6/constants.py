from collections import namedtuple

ClientTypeConstants = namedtuple('ClientType', ['XML_STRING',
                                        'REPORT',
                                        'COMPUTE'])
ClientType = ClientTypeConstants('client-type', 0, 1)

class XMLHier:
    """
    A dynamic way for our python code to organize the xml
    structure it receives/sends. 
    
    Note: much of this class is reflective of my time using 
    the built in golang xml parser. Therefore similarities
    are not coincidental.

    Reference http://golang.org/pkg/encoding/xml/
    """
    def __init__(self, XML_STRING=None, value=None):
        self.XML_STRING = XML_STRING
        self.value = None
        self._children = list()
    
    def add_child(self, XML_STRING, value=None):
        self._children.append(XMLHier(XML_STRING, value))

    def search(self, match_str):
        """
        Returns a field based on the match_str
        """
        pass

    def marshall(self):
        """
        Returns a string of the XML found in hierarchy
        """
        pass

    def unmarshall(self, string):
        """
        Converts xml string into this datastructure
        """
        pass

    def __getattr__(self, name):
        """
        Allow us to access children by XML_STRING
        """
        for child in self._children:
            if name == child.XML_STRING:
                return child
        raise AttributeError

