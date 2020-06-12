#include <cmath>
#include <iostream>

#define DEBUG

double STFT (double const* input_signal,
		int const n_samples,
		int const chunk_size,
		int const offset,
		double const freq)
{
	double real_prt = 0;
	double img_prt = 0;
	for(int m = 0; m < chunk_size; m++)
	{
		real_prt += input_signal[offset + m] * cos(2 * M_PI * m * freq / n_samples);
		img_prt += input_signal[offset + m] * sin(2 * M_PI * m * freq / n_samples);
	}


	#ifdef DEBUG
	std::cout << "Real part: " << real_prt << std::endl;
	std::cout << "Imaginary part: " << img_prt << std::endl;
	#endif

	return (real_prt * real_prt + img_prt * img_prt) / (n_samples * n_samples);
}

void unitTest()
{
	double sine_wave[1024];
	for(int i = 0; i < 1024; i++)
	{
		#ifdef DEBUG
		std::cout << sin(2 * M_PI * double(i) / 1024.0) << std::endl;
		#endif

		sine_wave[i] = sin(2 * M_PI * double(i) / 1024.0);
	}

	std::cout << STFT(sine_wave, 1024, 1024, 0, 1) << std::endl;	
	std::cout << STFT(sine_wave, 1024, 1024, 0, 2) << std::endl;
	std::cout << STFT(sine_wave, 1024, 256, 0, 1) << std::endl;
	std::cout << STFT(sine_wave, 1024, 256, 0, 2) << std::endl;
}

int main()
{
	unitTest();
}
