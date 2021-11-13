#include <iostream>

using namespace std;

uint8_t mgt[] = {
	0b1101,
	0b1011,
	0b1000,
	0b0111,
	0b0100,
	0b0010,
	0b0001
	};
	
uint8_t mh[] = {
	0b1010101,
	0b0110011,
	0b0001111
};

uint8_t mr[] = {
	0b0010000,
	0b0000100,
	0b0000010,
	0b0000001
};

uint8_t product(uint8_t m[], uint8_t v, uint8_t w, uint8_t h) {
	uint8_t ret = 0;
	for(uint8_t y = 0; y < h; y++)
	{
		ret = ret << 1;
		uint8_t mi = m[y];
		uint8_t e = 0;
		uint8_t b = 1;
		for(uint8_t x = 0; x < w; x++)
		{
			e = e + ((v & b) ? 1 : 0) * ((mi & b) ? 1 : 0);
			b = b << 1;
		}
		ret = ret | (e % 2);
	}
	return ret;
}

void binary_output(uint8_t c, uint8_t bits = 8) {
	for(uint8_t x = 0; x < bits; x++) {
		cout << (128 & c ? 1 : 0);
		c = c << 1;
	}
}

char* asBin(uint8_t c, uint8_t bits = 8) {
    char* ret = new char[bits + 1];
    for(uint8_t x = 0; x < bits; x++)
    {
        ret[x] = (128 & c ? '1' : '0');
        c = c << 1;
    }
    ret[bits] = '\0';
    return ret;
}

uint8_t encode(uint8_t b) {
	return product(mgt, b, 4, 7);
}

uint8_t decode(uint8_t r) {
	uint8_t z = product(mh, r, 7, 3);		    // syndrom
    uint8_t errorCount = 0;
    uint8_t b = z;
    for(uint8_t i = 0; i < 8; i++)
        if(1 & z)
            if((++errorCount) > 1)
                return 128;                     // If more than two errors found, return impossible value to indicate that repair is not possible
            
        
	uint8_t r2 = z ? r2 = r ^ 128 >> z : r;     // repair if error found
	return product(mr, r2, 7, 4);		        // decoded
}

inline void outputX(uint8_t a, uint8_t xa, uint8_t xm)
{
    char* ba = asBin(a, 8);
    char* bxa = asBin(xa, 8);
    char* bxm = asBin(xm, 8);
    cout << ba << " -> " << bxa << " -> " << bxm << (a != xm ? "\tERROR" : "") << "\n";        
    delete ba;
    delete bxa;
    delete bxm;    
}

int main() {

    for(uint8_t a = 0; a < 0b0001000; a++)
    {
        uint8_t xa = encode(a);
        uint8_t xm = decode(xa);
    
        outputX(a, xa, xm);
        
        // introduce error in one bit:
        
        for(uint8_t e = 0; e < 128; e++)
        {
            uint8_t xe = xa ^ e;
            uint8_t xem = decode(xe);
            if(xem != a)
            {
                cout << "  * ";
                outputX(a, xe, xem);
            }
        }
        
    }
    
    cout << "===================\n";
	
	uint8_t p = 0b00001011;					//initial data
	uint8_t x = encode(p);					// product(mgt, p, 4, 7);		//coded
	uint8_t r = 0b00110111;					//received
	uint8_t z = product(mh, r, 7, 3);		//syndrom
	uint8_t r2 = r;
	if(z)
		r2 = r ^ 128 >> z;
	
	uint8_t p2 = decode(r);
		
	//uint8_t p2 = product(mr, r2, 7, 4);		//decoded
    

	cout << "p = ";
	binary_output(p);
	cout << "\nx = ";
	binary_output(x);
	cout << "\nr = ";
	binary_output(r);
	cout << "\nz = ";
	binary_output(z);
	cout << "\nr2= ";
	binary_output(r2);
	
	
	cout << "\np2= ";
	binary_output(p2);	
	cout << "\n\n";
	
	
	return 0;
}