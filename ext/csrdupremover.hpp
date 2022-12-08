//This program removes duplicated entries from CSR arrays
//It can provide a complete set of duplicates-removed ap[], ai[] and ax[] for analysis, or duplicates-removed ax[] for per factorization/re-factorization
//Compile with -std=c++11 to use nullptr

#ifndef __CSRDUPREMOVER__
#define __CSRDUPREMOVER__

#include <string.h>
#include <stddef.h>

#define _int_t	int
//#define _int_t	long long

class CSRDuplicatesRemover
{
public:
	CSRDuplicatesRemover()
	{
		n_ = 0;
		oldnz_ = 0;
		newnz_ = 0;
		ap_ = nullptr;
		ai_ = nullptr;
		ax_ = nullptr;
		ptr_ = nullptr;
	}
	~CSRDuplicatesRemover()
	{
		delete []ap_;
		delete []ai_;
		delete []ax_;
		delete []ptr_;
	}
	//Call this function before analysis, then call ap(), ai() and ax() to get duplicates-removed CSR arrays
	//ax[] can be nullptr, if the values are unavailable when analysis
	bool ForAnalysis(_int_t n, const _int_t ap[], const _int_t ai[], const double ax[])
	{
		n_ = 0;
		oldnz_ = 0;
		newnz_ = 0;
		delete []ap_; ap_ = nullptr;
		delete []ai_; ai_ = nullptr;
		delete []ax_; ax_ = nullptr;
		delete []ptr_; ptr_ = nullptr;

		n_ = n;
		ap_ = new _int_t [n_ + 1];
		if (nullptr == ap_) return false;
		oldnz_ = ap[n];
		ai_ = new _int_t [oldnz_];
		if (nullptr == ai_) return false;
		ax_ = new double [oldnz_];
		if (nullptr == ax_) return false;
		ptr_ = new _int_t [oldnz_];
		if (nullptr == ptr_) return false;
		_int_t *flag = new _int_t [(size_t)n_ + n_];
		if (nullptr == flag) return false;
		memset(flag, -1, sizeof(_int_t) * n_);
		_int_t *pos = flag + n_;
		newnz_ = 0;
		ap_[0] = 0;

		if (ax != nullptr)
		{
			for (_int_t i = 0; i < n_; ++i)
			{
				const _int_t end = ap[i + 1];
				for (_int_t p = ap[i]; p < end; ++p)
				{
					const _int_t j = ai[p];
					if (flag[j] != i)
					{
						flag[j] = i;
						ai_[newnz_] = j;
						ax_[newnz_] = ax[p];
						ptr_[p] = newnz_;
						pos[j] = newnz_;
						++newnz_;
					}
					else
					{
						const _int_t tp = pos[j];
						ptr_[p] = tp;
						ax_[tp] += ax[p];
					}
				}
				ap_[i + 1] = newnz_;
			}
		}
		else
		{
			for (_int_t i = 0; i < n_; ++i)
			{
				const _int_t end = ap[i + 1];
				for (_int_t p = ap[i]; p < end; ++p)
				{
					const _int_t j = ai[p];
					if (flag[j] != i)
					{
						flag[j] = i;
						ai_[newnz_] = j;
						ptr_[p] = newnz_;
						pos[j] = newnz_;
						++newnz_;
					}
					else
					{
						const _int_t tp = pos[j];
						ptr_[p] = tp;
					}
				}
				ap_[i + 1] = newnz_;
			}
		}

		delete []flag;
		return true;
	}
	//Call this function before each factorization/re-factorization, then call ax() to get duplicates-removed CSR array
	//ForAnalysis must be called before the first call to this function
	bool ForFactorization(const double ax[])
	{
		memset(ax_, 0, sizeof(double) * newnz_);
		for (_int_t i = 0; i < oldnz_; ++i)
		{
			ax_[ptr_[i]] += ax[i];
		}
	}
	//Get duplicates-removed CSR arrays
	inline const _int_t *ap() const { return ap_; }
	inline const _int_t *ai() const { return ai_; }
	inline const double *ax() const { return ax_; }
	//Get duplicates-removed number of nonzeros
	inline const _int_t nnz() const { return newnz_; }

private:
	_int_t n_;
	_int_t oldnz_;
	_int_t newnz_;
	_int_t *ap_;
	_int_t *ai_;
	double *ax_;
	_int_t *ptr_;
};

#endif
