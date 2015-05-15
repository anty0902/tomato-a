
#include <opencv2\opencv.hpp>	// OpenCVヘッダのインクルード
#include <opencv2\nonfree\nonfree.hpp>

// OpenCVのバージョンを取得
#define CV_VER CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

// Debug, Releaceモードの切り替え
#ifdef _DEBUG
#define CV_EXT "d.lib"
#else
#define CV_EXT ".lib"
#endif

// 取得したバージョン文字列を付加したライブラリ名へリンク
#pragma comment(lib, "opencv_calib3d" CV_VER CV_EXT)
#pragma comment(lib, "opencv_contrib" CV_VER CV_EXT)
#pragma comment(lib, "opencv_core" CV_VER CV_EXT)
#pragma comment(lib, "opencv_features2d" CV_VER CV_EXT)
#pragma comment(lib, "opencv_flann" CV_VER CV_EXT)
#pragma comment(lib, "opencv_gpu" CV_VER CV_EXT)
#pragma comment(lib, "opencv_highgui" CV_VER CV_EXT)
#pragma comment(lib, "opencv_imgproc" CV_VER CV_EXT)
#pragma comment(lib, "opencv_legacy" CV_VER CV_EXT)
#pragma comment(lib, "opencv_ml" CV_VER CV_EXT)
#pragma comment(lib, "opencv_nonfree" CV_VER CV_EXT)
#pragma comment(lib, "opencv_objdetect" CV_VER CV_EXT)
#pragma comment(lib, "opencv_ocl" CV_VER CV_EXT)
#pragma comment(lib, "opencv_photo" CV_VER CV_EXT)
#pragma comment(lib, "opencv_stitching" CV_VER CV_EXT)
#pragma comment(lib, "opencv_superres" CV_VER CV_EXT)
#pragma comment(lib, "opencv_ts" CV_VER CV_EXT)
#pragma comment(lib, "opencv_video" CV_VER CV_EXT)
#pragma comment(lib, "opencv_videostab" CV_VER CV_EXT)

using namespace cv;	// OpenCV名前空間
using namespace std;

// 画素値取得用マクロ	RGBi:整数型　RGBf:少数型
#define Ri(IMG, X,Y) ((IMG).data[((IMG).step*(Y) + (IMG).channels()*(X)) + 2])
#define Gi(IMG, X,Y) ((IMG).data[((IMG).step*(Y) + (IMG).channels()*(X)) + 1])
#define Bi(IMG, X,Y) ((IMG).data[((IMG).step*(Y) + (IMG).channels()*(X)) + 0])

#define Rf(IMG, X,Y) (((Point3f*)((IMG).data + (IMG).step.p[0] * Y))[X].z)
#define Gf(IMG, X,Y) (((Point3f*)((IMG).data + (IMG).step.p[0] * Y))[X].y)
#define Bf(IMG, X,Y) (((Point3f*)((IMG).data + (IMG).step.p[0] * Y))[X].x)

// GYi:グレースケール　HSVi：HSV表色系
#define GYi(IMG, X,Y) ((IMG).data[((IMG).step*(Y) + (IMG).channels()*(X)) + 0])

#define Vi(IMG, X,Y) Ri(IMG, X,Y)
#define Si(IMG, X,Y) Gi(IMG, X,Y)
#define Hi(IMG, X,Y) Bi(IMG, X,Y)

