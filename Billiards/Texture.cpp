#include "stdafx.h"
#include "Texture.h"

char* GetFileEx(const char* path)
{
	char filePath[128];
	char *fileEx = NULL, *p, *tail;

	if (path == NULL)
		return "\0";
	strcpy_s(filePath, path);

	if ((tail = strchr(filePath, '\0')) == NULL)
		return "\0";
	for (p = tail; p >= filePath; p--)
		if (*p == '.')
		{
			fileEx = p + 1;
			break;
		}
	if (p < filePath)
		return "\0";

	for (p = fileEx; *p != '\0'; p++)
		*p = tolower(*p);

	return fileEx;
}

Texture::Texture()
{
	width = 0;
	height = 0;
	nChannels = 0;
	image = NULL;
}

Texture::~Texture()
{
	Release();
}

void Texture::Release()
{
	width = 0;
	height = 0;
	nChannels = 0;
	if (image)
	{
		delete[] image;
		image = NULL;
	}
}

bool Texture::Load(char* path)
{
	if (image)
		Release();

	if (!fileExists(path))
	{
		cout << "Texture Loader Error : Load() don't exist file " << path << endl;
		return false;
	}

	strcpy_s(filepath, path);
	char *fileEx = GetFileEx((const char*)path);

	int fileType = -1;
	if (!strcmp(fileEx, "sph") || !strcmp(fileEx, "spa"))
	{
		FILE* tmp_fp;
		byte head[2];
		if (fopen_s(&tmp_fp, path, "rb"))
		{
			cout << "Texture Loader Error : Load() can't open file " << path << endl;
			return false;
		}
		fread_s(head, 2, 2, 1, tmp_fp);
		if (head[0] == 0x89 && head[1] == 0x50)
			fileType = 1;
		else if (head[0] == 'B' && head[1] == 'M')
			fileType = 2;

		fclose(tmp_fp);
	}

	// png�`���̏ꍇ
	//if (!strcmp(fileEx, "png") || fileType == 1)
	if (fileType == 1)
	{
		vector<unsigned char> img; //the raw pixels
		unsigned w, h;
		//decode
		unsigned error = lodepng::decode(img, w, h, path);

		//if there's an error, display it
		if (error)
		{
			cout << "Texture Loader Error : LoadPng() error no" << error << ": " << lodepng_error_text(error) << endl;
			return false;
		}

		image = new byte[img.size()];
		for (unsigned int i = 0; i<img.size(); i++)
			image[i] = img.data()[i];

		width = w;
		height = h;
		if (img.size() == w*h * 4)
			nChannels = 4;
		else
			nChannels = 3;

		img.clear();
	}
	// bmp�`���̏ꍇ
	//else if (!strcmp(fileEx, "bmp") || fileType == 2)
	else if (fileType == 2)
	{
		FILE *fp;
		BITMAPFILEHEADER fileHeader;
		BITMAPINFOHEADER infoHeader;
		BITMAPCOREHEADER coreHeader;
		int bitCount;

		if (fopen_s(&fp, path, "rb"))
		{
			cout << "Texture Loader Error : LoadBmp() can't open file " << path << endl;
			return false;
		}

		fread_s(&fileHeader, sizeof(BITMAPFILEHEADER), sizeof(BITMAPFILEHEADER), 1, fp);
		if (fileHeader.bfType != 'B' + ('M' << 8))
		{
			cout << "Texture Loader Error : LoadBmp() file " << path << "is not Bitmap" << endl;
			fclose(fp);
			return false;
		}

		if (fileHeader.bfOffBits == 26)
		{
			fread_s(&coreHeader, sizeof(BITMAPCOREHEADER), sizeof(BITMAPCOREHEADER), 1, fp);
			width = coreHeader.bcWidth;
			height = coreHeader.bcHeight;
			bitCount = coreHeader.bcBitCount;
		}
		else if (fileHeader.bfOffBits == 54)
		{
			fread_s(&infoHeader, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), 1, fp);
			width = infoHeader.biWidth;
			height = infoHeader.biHeight;
			bitCount = infoHeader.biBitCount;
			if (infoHeader.biCompression != 0)
			{
				cout << "Texture Loader Error : LoadBmp() don't support commpressed Bitmap" << endl;
				fclose(fp);
				return false;
			}
		}
		else
		{
			cout << "Texture Loader Error : LoadBmp() don't support Bitmap with color pallete" << endl;
			fclose(fp);
			return false;
		}

		byte* buff = NULL;
		int line;
		int i, j;
		switch (bitCount)
		{
		case 24:
			line = width * 3;
			if (line % 4)
				line += (4 - line % 4);
			buff = new byte[line*abs(height)];
			fread_s(buff, line*abs(height), line*abs(height), 1, fp);
			image = new byte[width*abs(height) * 3];
			nChannels = 3;

			if (height>0)
			{
				for (j = 0; j<height; j++)
					for (i = 0; i<width; i++)
					{
						image[(j*width + i) * 3] = buff[(height - j - 1)*line + i * 3 + 2];
						image[(j*width + i) * 3 + 1] = buff[(height - j - 1)*line + i * 3 + 1];
						image[(j*width + i) * 3 + 2] = buff[(height - j - 1)*line + i * 3];
					}
			}
			else
			{
				height = -height;
				for (j = 0; j<height; j++)
					for (i = 0; i<width; i++)
					{
						image[(j*width + i) * 3] = buff[j*line + i * 3 + 2];
						image[(j*width + i) * 3 + 1] = buff[j*line + i * 3 + 1];
						image[(j*width + i) * 3 + 2] = buff[j*line + i * 3];
					}
			}

			break;

		case 32:
			buff = new byte[width*abs(height) * 4];
			fread_s(buff, width*abs(height) * 4, width*abs(height) * 4, 1, fp);
			image = new byte[width*abs(height) * 3];
			nChannels = 3;

			if (height>0)
			{
				for (j = 0; j<height; j++)
					for (i = 0; i<width; i++)
					{
						image[(j*width + i) * 3] = buff[((height - j - 1)*width + i) * 4 + 2];
						image[(j*width + i) * 3 + 1] = buff[((height - j - 1)*width + i) * 4 + 1];
						image[(j*width + i) * 3 + 2] = buff[((height - j - 1)*width + i) * 4];
					}
			}
			else
			{
				for (j = 0; j<height; j++)
					for (i = 0; i<width; i++)
					{
						image[(j*width + i) * 3] = buff[(j*width + i) * 4 + 2];
						image[(j*width + i) * 3 + 1] = buff[(j*width + i) * 4 + 1];
						image[(j*width + i) * 3 + 2] = buff[(j*width + i) * 4];
					}
			}

			break;

		default:
			cout << "Texture Loader Error : LoadBmp() don't support " << bitCount << " bit Bitmap" << endl;
			fclose(fp);
			return false;
		}
		if (buff)
			delete[] buff;

		fclose(fp);
	}
	// ���̑��̏ꍇ
	else {
		// TODO : opencv3�œǂݍ��ނ悤�ɂ���i���󂾂�RGB�œǂݍ��܂�Ă��܂��H�j
		IplImage* cvImage = cvLoadImage(path);
		int size = 0;

		width = cvImage->width;
		height = cvImage->height;
		nChannels = cvImage->nChannels;
		size = width*height*nChannels;

		if (cvImage)
		{
			image = new GLubyte[size];

			for (int i = 0; i < size; i += nChannels)
			{
				image[i] = cvImage->imageData[i + 2];        // R
				image[i + 1] = cvImage->imageData[i + 1];   // G
				image[i + 2] = cvImage->imageData[i];       //  B
				if (nChannels > 3)
				{
					image[i + 3] = cvImage->imageData[i + 3]; // A�����݂���ꍇ
				}
			}
			cvReleaseImage(&cvImage);
		}
		else
		{
			cout << "Texture Loader Error : failed to load image " << path << endl;
			return false;
		}
	}

	if (image == NULL)
		return false;

	else if (nChannels<3)
	{
		cout << "Texture Loader Error : Load() don't support " << nChannels * 8 << "bit picture" << endl;
		return false;
	}
	else
		return true;
}

bool Texture::CreateTexture(GLuint* imageTex)
{
	if (nChannels != 3 && nChannels != 4)
	{
		*imageTex = 0xFFFFFFFF;
		return false;
	}

	glGenTextures(1, imageTex);
	glBindTexture(GL_TEXTURE_2D, *imageTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (nChannels == 3)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, (GLuint)width, (GLuint)height, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (nChannels == 4)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, (GLuint)width, (GLuint)height, GL_RGBA, GL_UNSIGNED_BYTE, image);

	return true;
}