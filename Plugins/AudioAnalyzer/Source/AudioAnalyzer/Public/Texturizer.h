// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#pragma once

#include <memory>
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Texturizer.generated.h"

/**
* @brief Texturizer
*
* Tool to convert analysis values into a Texture2D
*/
UCLASS(Blueprintable, BlueprintType)
class UTexturizer : public UObject
{
	GENERATED_BODY()
public:

	/**
	* Texture2D result
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		UTexture2D* dynamicCanvas;

	/** @name Tools	*/
	/**@{*/
	/**
	* Initializes the texture canvas
	* @param Width		Horizontal pixels
	* @param Height		Vertical pixels
	*/
	UFUNCTION(BlueprintCallable, Category = Tools)
		void InitializeCanvas(const int32 Width, const int32 Height);

	/**
	* Force update texture canvas
	*/
	UFUNCTION(BlueprintCallable, Category = Tools)
		void UpdateCanvas();

	/**
	* Reset texture canvas
	*/
	UFUNCTION(BlueprintCallable, Category = Tools)
		void ClearCanvas();

	/**
	* Convert float array into canvas pixels
	* @param floats		Float container
	*/
	UFUNCTION(BlueprintCallable, Category = Tools)
		void FloatsToCanvas(const TArray<float>& floats);

	/**
	* Convert int32 array into canvas pixels
	* @param ints		Int32 container
	*/
	UFUNCTION(BlueprintCallable, Category = Tools)
		void IntsToCanvas(const TArray<int32>& ints);
	/**@}*/

	/**
	* Construcrtor
	*/
	UTexturizer();

	/**
	* Destructor
	*/
	~UTexturizer();

private:

	/**
	* Canvas pixel data
	*/
	std::unique_ptr<uint8[]> canvasPixelData;

	/**
	* Canvas width (pixels)
	*/
	int canvasWidth;

	/**
	* Canvas height (pixels)
	*/
	int canvasHeight;

	/**
	* Bytes per pixel
	*/
	int bytesPerPixel;

	/**
	* Canvas Buffer Pitch
	*/
	int bufferPitch;

	/**
	* Canvas Buffer Size
	*/
	int bufferSize;

	/**
	* Specifies an update region for the canvas texture
	*/
	std::unique_ptr<FUpdateTextureRegion2D> echoUpdateTextureRegion;

	/**
	* Set the color value of one pixel of the canvas
	* @param pointer			Pointer to the canvas pixel
	* @param red				Pixel red value
	* @param green				Pixel green value
	* @param blue				Pixel blue value
	* @param alpha				Pixel alpha value
	*/
	void setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha);

};
