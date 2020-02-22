#include "ImageIO.hpp"
enum ColorSpace
{
	RGB,
	GRAY,
	HSV
};
class ColorTrans
{
private:
    ColorSpace Color;
	HWND ParenthWnd;
	std::string Imgextension;
	const IO *obj;
	Mat temp;

	inline void Write2Tmp()const;
public:
	ColorTrans(HWND hwnd,const IO *io);
	ColorTrans(HWND hwnd, const IO io);
	void RGB2Gray();
	void RGB2HSV();
	void HSV2RGB();
	void Gray2RGB();
	MatND CalcHistogram()const;
	void DisplayHistogram(MatND hist)const;
	void HistBalance();
	void ShowEdge(int code)const;
	void Threshold(int thval);
	ColorSpace GetColorSpace()const { return this->Color; }
};