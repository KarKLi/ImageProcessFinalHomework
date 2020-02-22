#include "ImageFT.hpp"

void FT::dft()
{
	m = getOptimalDFTSize(Img.rows);
	n = getOptimalDFTSize(Img.cols);
	copyMakeBorder(Img, padded, 0, m - (Img.rows), 0, n - (Img.cols), BORDER_CONSTANT, Scalar::all(0));//enlarge the border
	planes[0] = Mat_<float>(padded);
	planes[1] = Mat::zeros(padded.size(), CV_32F);
	merge(planes, 2, complexI);
	cv::dft(complexI, complexI);
	split(complexI, planes);//Divide the real part and imaginary part
	Mat amplitude;
	magnitude(planes[0], planes[1], amplitude);//planes[0] contains the real part and the planes[1] contains the imaginary part
	magnitudeImg = amplitude;
	magnitudeImg += Scalar::all(1);
	log(magnitudeImg, magnitudeImg);//the log transform, formula:log(1+x) x \in D
	magnitudeImg = magnitudeImg(Rect(0, 0, magnitudeImg.cols&-2, magnitudeImg.rows&-2));
	normalize(magnitudeImg, magnitudeImg, 0, 1, CV_MINMAX);
}

void FT::showCentralAmplitude()const
{
	int cx = magnitudeImg.cols / 2;
	int cy = magnitudeImg.rows / 2;

	Mat q0(magnitudeImg, Rect(0, 0, cx, cy));
	//imshow("What does q0 look like?",q0);//White
	Mat q1(magnitudeImg, Rect(cx, 0, cx, cy));
	Mat q2(magnitudeImg, Rect(0, cy, cx, cy));
	Mat q3(magnitudeImg, Rect(cx, cy, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(magnitudeImg, magnitudeImg, 0, 1, CV_MINMAX);
	imshow("中心化幅度图", magnitudeImg);
}