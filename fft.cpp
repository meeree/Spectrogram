#include <cmath>
#include <iostream>

double STFT (double const* input_signal,
		int const chunk_size,
		int const offset,
		double const freq)
{
	double real_prt = 0;
	double img_prt = 0;
	for(int m = 0; m < chunk_size; m++)
	{
		real_prt += input_signal[offset + m] * cos(2 * M_PI * m * freq / chunk_size);
		img_prt += input_signal[offset + m] * sin(2 * M_PI * m * freq / chunk_size);
	}
	return real_prt * real_prt + img_prt * img_prt;
}

void unitTest()
{
	double sine_wave[1024];
	for(int i = 0; i < 1024; i++)
	{
		std::cout << sin(2 * M_PI * double(i) / 1024.0) << std::endl;
		sine_wave[i] = sin(2 * M_PI * double(i) / 1024.0);
	}
	
	std::cout << STFT(sine_wave, 1024, 0, 1) << std::endl;
}

int main()
{
	unitTest();
}
