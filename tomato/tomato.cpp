#include "stdafx.h"
#include "opencv_lib.hpp"	
#include <math.h>                // OpenCVヘッダ
#include <time.h>
// change 4(by nagata)(anty0902)
#define PI (atan(1.0)*4)

int Hmin = 5, Hmax = 35;										// 色相初期値
CvPoint *point, *tmp;
// トラックバー呼び出し関数
void onTrackbarS(int position, void* val){ Hmin = position; }	// 色相（最小値）
void onTrackbarL(int position, void* val){ Hmax = position; }	// 色相（最大値）

int _tmain(int argc, _TCHAR* argv[])
{
	//	int B_KEY = 1;
	Mat frame, img_hsv, img_hue, img_mask, img_out, img_tomato, drawing, dst_img, canny_output;	// 画像リソース宣言
	double angle, longline, shortline, medle;
	int w, h, x, y, i, j;
	vector<vector<Point> > contours, imgout;
	vector<Vec4i> hierarchy;
	Point2f vertices[4], center, after[2], midle;
	RotatedRect box;

	img_tomato = imread("tomato1.png");
	resize(img_tomato, img_tomato, cv::Size(), 0.5, 0.5); //画像サイズの変換(＊キャリブレーション必要＊)
	Mat img2(Size(1280, 900), img_tomato.type(), Scalar(0, 0, 0));  //背景の設定
	imshow("画像表示", img2);

	waitKey(0);									// キー入力待機

	//カメラから読み込む
	VideoCapture src(1);
	src.set(CV_CAP_PROP_SATURATION, 100); //彩度をあがる
	cout << src.get(CV_CAP_PROP_SATURATION) << endl;
	Scalar color_red = Scalar(0, 0, 255);
	double area = 0.0;

	if (src.isOpened() == 0){ cout << "映像が取得できません。\n" << endl; waitKey(0); return -1; }

	namedWindow("色相", 0);
	createTrackbar("H (MIN)", "色相", &Hmin, 180, onTrackbarS);	// 色相トラックバーの設定
	setTrackbarPos("H (MIN)", "色相", Hmin);
	createTrackbar("H (MAX)", "色相", &Hmax, 180, onTrackbarL);
	setTrackbarPos("H (MAX)", "色相", Hmax);

	while (frame.data == NULL){ src >> frame; }					// 初期フレーム取得
	w = frame.cols, h = frame.rows;

	// 画像リソースの確保
	img_hue.create(Size(180, 60), CV_8UC3);
	img_hsv.create(frame.size(), CV_8UC3);
	img_out.create(frame.size(), CV_8UC3);
	img_mask.create(frame.size(), CV_8UC1);

	for (y = 0; y < 60; y++){								// 色相カラーバーの描画
		for (x = 0; x < 180; x++){
			Hi(img_hue, x, y) = x;								// H（色相）　色相のみを変化させる（0～180）
			Si(img_hue, x, y) = 255;							// S（彩度）
			Vi(img_hue, x, y) = 255;							// V（明度）
		}
	}
	cvtColor(img_hue, img_hue, CV_HSV2BGR);						// 色相変化画像の表色系変換（HSV→BGR）
	imshow("色相", img_hue);


	while (1){

		int64 start = getTickCount();                            //スタート時間

		src >> frame; if (frame.data == NULL) break;			// 1フレーム取得
		GaussianBlur(frame, frame, Size(15, 15), 15.0, 15.0, 4); //　ぼやけてする
		cvtColor(frame, img_hsv, CV_BGR2HSV);					// 入力フレームの表色系変換（BGR→HSV）
		//HmaxからHminまでの色を抽出
		for (y = 0; y < h; y++){
			for (x = 0; x < w; x++){
				if (Hmin <= Hi(img_hsv, x, y) && Hi(img_hsv, x, y) <= Hmax){
					GYi(img_mask, x, y) = 0;						// H(min)≦H≦H(max)のマスクを0
				}
				else {											// H(min)≦H≦H(max)以外を255（透過）
					GYi(img_mask, x, y) = 255;
				}
			}
		}

		morphologyEx(img_mask, img_mask, MORPH_CLOSE, Mat(), Point(-1, -1), 6);// ノイズ除去（closing）
		morphologyEx(img_mask, img_mask, MORPH_OPEN, Mat(), Point(-1, -1), 4);// ノイズ除去（opening）

		//輪郭を描画
		Canny(img_mask, canny_output, 80, 80 * 2, 3);/// Detect edges using canny
		findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1, Point(0, 0));	/// Find contours
		drawing = Mat::zeros(canny_output.size(), CV_8UC3);/// Draw contours

		//輪郭面積
		if (contours.size() != 0)
		{
			area = contourArea(contours[0]);
			//			printf("area(%f)\n", area);
		}

		Point2f vertices[4];
		if ((area > 10000) && (contours.size() != 0))
		{
			drawContours(drawing, contours, 0, color_red, 2, 8, hierarchy, 0, Point()); //楕円を描画		
			RotatedRect box = minAreaRect(contours[0]); //矩形を描画
			box.points(vertices);
			//       	area = contourArea(contours[i]);
			//			printf("area(%f)\n", area);
			for (j = 0; j < 4; ++j)
			{
				line(drawing, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 1, CV_AA);
				//				printf("頂点座標(%d,%d)\n", (int)vertices[j].x, (int)vertices[j].y);  //矩形の頂点
			}
		}

		//中心座標
		center.x = (vertices[0].x + vertices[1].x + vertices[2].x + vertices[3].x) / 4;
		center.y = (vertices[0].y + vertices[1].y + vertices[2].y + vertices[3].y) / 4;
		circle(drawing, center, 4, color_red, -1, 8, 0);
		//		printf("center(%d,%d)\n", (int)center.x, (int)center.y);

		//矩形の長さ,高さ
		shortline = sqrt(pow(vertices[0].x - vertices[1].x, 2.0) + pow(vertices[0].y - vertices[1].y, 2.0));
		longline = sqrt(pow(vertices[0].x - vertices[3].x, 2.0) + pow(vertices[0].y - vertices[3].y, 2.0));
		if (shortline > longline)
		{
			medle = longline;
			longline = shortline;
			shortline = medle;
		}
		//		printf("lengthlong(%d),lengthshort(%d)\n", (int)longline, (int)shortline);

		//回転後の座標
		j = 0;
		for (i = 0; i < 4; i++)
		{
			if (vertices[i].x > center.x)
			{
				after[j] = vertices[i];
				j++;
			}
			else if (vertices[i].x == center.x)
			{
				if (vertices[i].y > center.y){
					after[j] = vertices[i];
					j++;
				}
			}
		}
		midle.x = ((after[0].x + after[1].x) / 2 - center.x);
		midle.y = (center.y - (after[0].y + after[1].y) / 2);

		//ケチャップの回転
		angle = (atan2((int)midle.y, (int)midle.x) / PI * 180);
		Point2f  center_tomato((float)(img_tomato.cols*0.5), (float)(img_tomato.rows*0.5));// 画像中心
		const Mat affine_matrix = getRotationMatrix2D(center_tomato, angle, 1.0);
		//		cout << "affine_matrix=\n" << affine_matrix << std::endl;
		warpAffine(img_tomato, dst_img, affine_matrix, img_tomato.size());

		if ((area > 20000) && (contours.size() != 0))
		{
			//回転後の画像を改めて載せる
			img2.setTo(0);
			Rect roi((int)center.x + 150 + ((int)center.x - 190)*0.5 - img_tomato.cols / 2, (int)center.y + 180 + ((int)center.y - 105)*0.6 - img_tomato.rows / 2, img_tomato.cols, img_tomato.rows);
			Rect roi2((int)center.x - img_tomato.cols / 2, (int)center.y - img_tomato.rows / 2, img_tomato.cols, img_tomato.rows);
			Mat img2_roi = img2(roi);
			Mat img2_roi2 = frame(roi2);
			dst_img.copyTo(img2_roi, dst_img);
			dst_img.copyTo(img2_roi2, dst_img);
		}

		int64 end = getTickCount();                                //end時間
		std::cout << (end - start) * 1000 / cv::getTickFrequency() << "[ms]" << std::endl;     //roop時間の計算

		// 1フレーム表示
		imshow("輪郭映像", drawing);
		imshow("マスク映像", img_mask);
		imshow("入力映像", frame);
		imshow("画像表示", img2);
		imshow("ケチャップ映像", dst_img);

		if (waitKey(1) == 27) break;			// キー入力待機（30ms）


	}

	return 0;
}
