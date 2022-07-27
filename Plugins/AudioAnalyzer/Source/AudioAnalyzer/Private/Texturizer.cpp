// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "Texturizer.h"

UTexturizer::UTexturizer()
{
}

UTexturizer::~UTexturizer()
{
}

void UTexturizer::InitializeCanvas(const int32 Width, const int32 Height)
{
	//dynamic texture initialization
	canvasWidth = Width;
	canvasHeight = Height;

	dynamicCanvas = UTexture2D::CreateTransient(canvasWidth, canvasHeight);
#if WITH_EDITORONLY_DATA
	dynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	dynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicCanvas->SRGB = 1;
	dynamicCanvas->AddToRoot();
	dynamicCanvas->Filter = TextureFilter::TF_Nearest;
	dynamicCanvas->UpdateResource();

	echoUpdateTextureRegion = std::unique_ptr<FUpdateTextureRegion2D>(new FUpdateTextureRegion2D(0, 0, 0, 0, canvasWidth, canvasHeight));


	// buffers initialization
	bytesPerPixel = 4; // r g b a
	bufferPitch = canvasWidth * bytesPerPixel;
	bufferSize = canvasWidth * canvasHeight * bytesPerPixel;
	canvasPixelData = std::unique_ptr<uint8[]>(new uint8[bufferSize]);

	ClearCanvas();
}

void UTexturizer::FloatsToCanvas(const TArray<float>& floatsContainer)
{
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int freqIndex = 0; freqIndex < floatsContainer.Num(); ++freqIndex)
	{
		uint8 colorValue = static_cast<int>(floatsContainer[freqIndex] * 255);

		//function to distribute the values over the surface
		int32 tx = freqIndex % canvasWidth;
		int32 ty = freqIndex / canvasWidth;

		//update texture values
		if (tx >= 0 && tx < canvasWidth && ty >= 0 && ty < canvasHeight)
		{
			canvasPixelPtr = canvasPixelData.get() + (tx + ty * canvasWidth) * bytesPerPixel;
			setPixelColor(canvasPixelPtr, colorValue, colorValue, colorValue, 255);
		}
	}

	UpdateCanvas();
}

void UTexturizer::IntsToCanvas(const TArray<int32>& intContainer)
{
	uint8* canvasPixelPtr = canvasPixelData.get();

	memset(canvasPixelPtr, 0, bufferSize);

	for (int sampleIndex = 0; sampleIndex < intContainer.Num(); ++sampleIndex)
	{
		int32 value = intContainer[sampleIndex];

		uint8 rvalue = 0;
		uint8 gvalue = 0;
		uint8 bvalue = 0;

		if (value == 3)
		{
			rvalue = 255;
			gvalue = 0;
			bvalue = 0;
		}
		else if (value == 2)
		{
			rvalue = 0;
			gvalue = 0;
			bvalue = 255;
		}
		else if (value == 1)
		{
			rvalue = 180;
			gvalue = 180;
			bvalue = 180;
		}

		for (int pixelH = 0; pixelH < value; ++pixelH)
		{
			//update texture values
			if (sampleIndex >= 0 && sampleIndex < canvasWidth && pixelH >= 0 && pixelH < canvasHeight)
			{
				canvasPixelPtr = canvasPixelData.get() + (sampleIndex + pixelH * canvasWidth) * bytesPerPixel;
				setPixelColor(canvasPixelPtr, rvalue, gvalue, bvalue, 255);
			}
		}
	}

	UpdateCanvas();
}

void UTexturizer::ClearCanvas()
{
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int i = 0; i < canvasWidth * canvasHeight; ++i)
	{
		setPixelColor(canvasPixelPtr, 0, 0, 0, 255); //black
		canvasPixelPtr += bytesPerPixel;
	}
	UpdateCanvas();
}

void UTexturizer::UpdateCanvas()
{
	if (echoUpdateTextureRegion)
	{
		dynamicCanvas->UpdateTextureRegions((int32)0, (uint32)1, echoUpdateTextureRegion.get(), (uint32)bufferPitch, (uint32)bytesPerPixel, canvasPixelData.get());
	}
}

void UTexturizer::setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	*pointer = blue; //b
	*(pointer + 1) = green; //g
	*(pointer + 2) = red; //r
	*(pointer + 3) = alpha; //a
}