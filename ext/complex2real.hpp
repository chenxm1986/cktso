//This program provides conversion from complex-number matrix to real-number matrix
//Complex numbers are stored using double arrays in interleaved real-imag form
//General conversion rule: a+jb ==> [a, -b; b, a]
//Compile with -std=c++11 to use nullptr

#ifndef __COMPLEX_TO_REAL__
#define __COMPLEX_TO_REAL__

#include <string.h>
#include <stddef.h>

#define _int_t	int
//#define _int_t	long long

class ComplexToReal
{
public:
	ComplexToReal()
	{
		n_ = 0;
		ap_ = nullptr;
		ai_ = nullptr;
		ax_ = nullptr;
	}
	~ComplexToReal()
	{
		delete []ap_;
		delete []ai_;
		delete []ax_;
	}
	//Call this function before analysis, then call n(), ap(), ai() and ax() to get converted real-number CSR arrays
	bool ForAnalysis(_int_t n, const _int_t cp[], const _int_t ci[], const double cx[])
	{
		n_ = 0;
		delete []ap_; ap_ = nullptr;
		delete []ai_; ai_ = nullptr;
		delete []ax_; ax_ = nullptr;

		n_ = n;
		const _int_t rn = n_ + n_;
		const _int_t rnz = cp[n_] * 4;

		ap_ = new _int_t [rn + 1];
		if (nullptr == ap_) return false;
		ai_ = new _int_t [rnz];
		if (nullptr == ai_) return false;
		ax_ = new double [rnz];
		if (nullptr == ax_) return false;

		ap_[0] = 0;
		for (_int_t i = 0; i < n_; ++i)
		{
			const _int_t i2 = i + i;
			const _int_t start = cp[i];
			const _int_t end = cp[i + 1];
			const _int_t len = (end - start) * 2;
			_int_t start2 = ap_[i2];
			ap_[i2 + 1] = start2 + len;
			ap_[i2 + 2] = ap_[i2 + 1] + len;
			for (_int_t p = start; p < end; ++p, start2 += 2)
			{
				const _int_t j = ci[p];
				const _int_t j2 = j + j;
				const double real = cx[p + p];
				const double imag = cx[p + p + 1];

				const _int_t a = start2;
				const _int_t b = a + 1;
				const _int_t c = a + len;
				const _int_t d = c + 1;

				ai_[a] = j2;
				ai_[b] = j2 + 1;
				ai_[c] = j2;
				ai_[d] = j2 + 1;

				ax_[a] = real;
				ax_[b] = -imag;
				ax_[c] = imag;
				ax_[d] = real;
			}
		}

		return true;
	}
	//Call this function before each factorization/re-factorization, then call ax() to get converted real-number CSR array
	//ForAnalysis must be called before the first call to this function
	bool ForFactorization(const double cx[])
	{
		for (_int_t i = 0; i < n_; ++i)
		{
			const _int_t start = ap_[i + i] / 2;
			const _int_t end = ap_[i + i + 2] / 2;
			const _int_t len = end - start;
			_int_t start2 = ap_[i + i];
			for (_int_t p = start; p < end; p += 2, start2 += 2)
			{
				const double real = cx[p];
				const double imag = cx[p + 1];

				const _int_t a = start2;
				const _int_t b = a + 1;
				const _int_t c = a + len;
				const _int_t d = c + 1;

				ax_[a] = real;
				ax_[b] = -imag;
				ax_[c] = imag;
				ax_[d] = real;
			}
		}

		return true;
	}
	//Get converted real-number CSR arrays
	inline _int_t n() const { return n_ + n_; }
	inline const _int_t *ap() const { return ap_; }
	inline const _int_t *ai() const { return ai_; }
	inline const double *ax() const { return ax_; }

private:
	_int_t n_;
	_int_t *ap_;
	_int_t *ai_;
	double *ax_;
};

#endif
