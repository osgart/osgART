///////////////////////////////////////////////////////////////////////////////
// File name : IntranelStreamVideo.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O problem with the GetPin: if I removed the MSG TXT, it can't find the pin..weird
//    need replace static resolution with dynamic (because intranel box can change res)
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include "IntranelStreamVideo"

using namespace std;
using namespace osgART;

///////////////////////////////////////////////////////////////////////////////
// Macro 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// class IntranelStreamVideo
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Msg: Display an error message box if needed
//-----------------------------------------------------------------------------

void Msg(TCHAR *szFormat, ...)
{
    TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
    const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    const int LASTCHAR = NUMCHARS - 1;

    // Format the input string
    va_list pArgs;
    va_start(pArgs, szFormat);

    // Use a bounded buffer size to prevent buffer overruns.  Limit count to
    // character size minus one to allow for a NULL terminating character.
    _snprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
    va_end(pArgs);

    // Ensure that the formatted string is NULL-terminated
    szBuffer[LASTCHAR] = TEXT('\0');

	MessageBox(NULL,szBuffer,"tpDS Message",MB_OK | MB_ICONERROR);

}

///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////

struct __declspec(uuid("{D7A2CE2F-8221-4b80-B086-B795D9C845F5}")) CLSID_DSMemoryRenderer;

class DSMemoryRenderer : public CBaseVideoRenderer
{
public:
    DSMemoryRenderer(tpVideoFrame* , IntranelStreamVideo* , LPUNKNOWN pUnk,HRESULT *phr);
    ~DSMemoryRenderer();

public:

    HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
    HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
    HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample


private:
                                                    // then we will use Blt from m_pSurfBuf to 
                                                    // m_pTexture to provide format translation
	IntranelStreamVideo* m_base;
    tpVideoFrame* m_frame;

	bool m_updated;		// Check 

    LONG m_lVidWidth;   // Video width
    LONG m_lVidHeight;  // Video Height
    LONG m_lVidPitch;   // Video Pitch
};


DSMemoryRenderer::DSMemoryRenderer(tpVideoFrame* frame,IntranelStreamVideo* base , 
								   LPUNKNOWN pUnk, HRESULT *phr )
	: CBaseVideoRenderer(__uuidof(CLSID_DSMemoryRenderer), 
		NAME("DSMemoryRenderer"), 
		pUnk, phr),
		m_frame(frame),
		m_base(base)
{
}

CCritSec g_cs;


DSMemoryRenderer::~DSMemoryRenderer()
{
    // delete [] m_frame->buffer;
	// m_frame->buffersize = 0;
};

HRESULT DSMemoryRenderer::CheckMediaType(const CMediaType *pmt)
{

	CAutoLock lock(&g_cs);
	
    HRESULT   hr = E_FAIL;
    VIDEOINFO *pvi=0;
    
    CheckPointer(pmt,E_POINTER);

    try
    {
        if( *pmt->FormatType() != FORMAT_VideoInfo ) 
        {
            return E_INVALIDARG;
        }

        // Only accept RGB32
        pvi = (VIDEOINFO *)pmt->Format();

        if( IsEqualGUID( *pmt->Type(), MEDIATYPE_Video) )
        {
            hr = S_OK;

			if( IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB32) )
            {
              //  m_frame->format = TP_RGBA;
				m_frame->width = pvi->bmiHeader.biWidth;
				m_frame->height = pvi->bmiHeader.biHeight;
            }
            else
            {
                hr = DDERR_INVALIDPIXELFORMAT;
            }
        };
        
    }// try
    catch(...)
    {
       // Msg(TEXT("Failed to check media type in the renderer. Unhandled exception hr=0x%x"), E_UNEXPECTED);
        hr = E_UNEXPECTED;
    };
	
    return hr;
}



//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT DSMemoryRenderer::SetMediaType(const CMediaType *pmt)
{
	CAutoLock lock(&g_cs);

    HRESULT hr = S_OK;
    VIDEOINFO *pviBmp = NULL;   // Bitmap info header

    try
    {
        // Retreive the size of this media type
        pviBmp = (VIDEOINFO *)pmt->Format();

        m_lVidWidth  = pviBmp->bmiHeader.biWidth;
        m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
        // m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24


		m_frame->width = m_lVidWidth;
		m_frame->height = m_lVidHeight;


    }// try
    catch(...)
    {
        //Msg(TEXT("Failed to set media type in the renderer. Unhandled exception hr=0x%x"), E_UNEXPECTED);
        return hr;
    }

    return hr;
}

// code from NeHe tutorial 35 (+ fallback solution)
void tpFlipRGB(void* buffer,size_t buffersize,int width)										
{
#ifdef _MSC_VER
	__asm														
	{
		mov ecx, buffersize										
		mov ebx, dword ptr[buffer]								
		label:													
			mov al,[ebx+0]										
			mov ah,[ebx+2]										
			mov [ebx+2],al										
			mov [ebx+0],ah										
			
			add ebx,width											
			dec ecx												
			jnz label											
	}
#else

	register unsigned char swp;
	register unsigned char* buf = (unsigned char*)buffer;

	for (size_t i = 0;i < buffersize;++i)
	{
		swp = *buffer[i*width];
		*buffer[i*width] = *buffer[i*width+2];
		*buffer[i*width+2] = swp;
	};

#endif
}

HRESULT DSMemoryRenderer::DoRenderSample( IMediaSample * pSample )
{

	CAutoLock lock(&g_cs);

    HRESULT hr = S_OK;

	BYTE* pSampleBuffer = 0;

    try
    {
		//Msg(TEXT("yoyo..\n"));   
		long sampleSize = pSample->GetSize();

		//fprintf(stderr,"before get pointer..\n");
		hr = pSample->GetPointer( &pSampleBuffer );
		//fprintf(stderr,"get image..");
		//fprintf(stderr,"get buffer=%i size=%i\n",pSampleBuffer,sampleSize);
		if ((!m_frame->buffer) || ((long)m_frame->buffersize < sampleSize))
		{
			if (m_frame->buffer) delete [] m_frame->buffer;
			m_frame->buffer = new unsigned char[sampleSize];
			m_frame->buffersize = sampleSize;
		};

		// Tests witch MMX enabled memcpy have unveiled that
		// there is no reason to clutter the sources with
		// those things - the gain is minimal
		
		m_base->CheckOut();
		memcpy(m_frame->buffer,pSampleBuffer,sampleSize);
		//tpFlipRGB(m_frame->buffer,m_frame->width * m_frame->height,4);
		m_base->CheckIn();
		
    }
    catch(...)
    {
    
    }
    return hr;
}



///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

IntranelStreamVideo::IntranelStreamVideo():m_frame(0),
	managed(false),
	m_pCapture(NULL)
{
	m_frame=new tpVideoFrame;
	m_frame->buffer=NULL;
	m_frame->buffersize=0;
	pixelsize=4;
	pixelformat=VIDEOFORMAT_BGRA32;
	xsize=720;
	ysize=576;
}

IntranelStreamVideo::IntranelStreamVideo(const IntranelStreamVideo &)
{
    
}

IntranelStreamVideo::~IntranelStreamVideo(void)
{
	this->stop();
	this->close();
}

IntranelStreamVideo& 
IntranelStreamVideo::operator=(const IntranelStreamVideo &)
{
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void
IntranelStreamVideo::open()
{

	DSMemoryRenderer* pDMR =0;
	HRESULT hr = S_OK;

	CoInitialize(NULL);

	hr = CoCreateInstance((REFCLSID)CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder,(void**)&m_pGB);
	if( FAILED(hr))
	{
		//Msg(TEXT("Failed to create filter graph.\nhr = 0x%08x"), hr);
		exit(-1);
	}

	pDMR = new DSMemoryRenderer(m_frame,this,NULL,&hr);

	if (FAILED(hr) || !pDMR)
	{
		std::cerr<<"ERROR:Can't create a DsMemoryRenderer.."<<std::endl;
		exit(-1);
	}

	m_pRenderer = pDMR;

	hr = m_pGB->AddFilter(m_pRenderer, L"Memory Renderer");
	if (FAILED(hr))
	{
		//Msg(TEXT("Could not add renderer filter to graph.  hr=0x%x"), hr);
		exit(-1);
	};

	CaptureVideo(m_pRenderer);
	
}

void
IntranelStreamVideo::close()
{

	OAFilterState _filter_state;

	HRESULT _r = m_pMC->GetState(100,&_filter_state);

	if (FAILED(_r) || _filter_state != State_Stopped)
	{
		if (FAILED(_r = m_pMC->StopWhenReady())) _r = m_pMC->Stop();
	};

	
	//	The following code is needed to get rid of all filters in
	//	the graph. 
	 

	// Enumerate the filters in the graph.
	IEnumFilters *pEnum = NULL;
	HRESULT hr = m_pGB->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		IBaseFilter *pFilter = NULL;
		while (S_OK == pEnum->Next(1, &pFilter, NULL))
		{
			 // Remove the filter.
			 m_pGB->RemoveFilter(pFilter);
			 // Reset the enumerator.
			 pEnum->Reset();
			 pFilter->Release();
		}
		pEnum->Release();
	};
	
}

void
IntranelStreamVideo::start()
{
if (m_pMC)
	{
		// ok, there is some evil thing about IMediaControl::Run()
		if (m_pMC->Run() == S_FALSE)
		{
			long pfs;
			m_pMC->GetState(100,&pfs);
			if (pfs != 2) 
			{
				std::cerr<<"ERROR:can't start the video stream.."<<std::endl;
				exit(-1);
			}

		} 
	}
else
{
	std::cerr<<"ERROR:can't start the video stream.."<<std::endl;
	exit(-1);
}
}

void
IntranelStreamVideo::stop()
{
	if (m_pMC)
		if (m_pMC->StopWhenReady() != S_OK)
		{
			std::cerr<<"ERROR:can't stop the video stream.."<<std::endl;
			exit(-1);
		}
}


IBaseFilter *GetFilter(const char* name_filter)
{
	HRESULT hr = S_OK;

	IBaseFilter *myFilter=NULL;
	
	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		Msg(TEXT("Error can't create sys enumerator.."));
	} 
	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &pEnumCat, 0);

	if (hr == S_OK) 
	{
    // Enumerate the monikers.
    IMoniker *pMoniker = NULL;
    ULONG cFetched;
    while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
    {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
            (void **)&pPropBag);
        if (SUCCEEDED(hr))
        {
            // To retrieve the filter's friendly name, do the following:
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if (SUCCEEDED(hr))
            {
				char szName[64];
				WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, szName, 64, 0, 0);
				if (strcmp(szName,name_filter)==0)
				{
					fprintf(stderr,"%s found..\n",szName);
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,(void**)&myFilter);
					if (SUCCEEDED(hr))
					{
						//Msg(TEXT("Find RTSP Filter.."));
					}
					else
					{
						Msg(TEXT("Stupid code.."));
					}
				}
            }
            VariantClear(&varName);
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
}
pSysDevEnum->Release();

return myFilter;

}


HRESULT IntranelStreamVideo::CaptureVideo(IBaseFilter *pRenderer)
{
	
    HRESULT hr = S_OK;
    CComPtr<IBaseFilter> pSrcFilter;
	CComPtr <IAMStreamConfig> pSrcConfig = NULL;

    // Create the capture graph builder object to assist in building
    // the video capture filter graph
    hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                           IID_ICaptureGraphBuilder2, (void **) &(m_pCapture));
    if (FAILED(hr))
    {
        Msg(TEXT("Could not create the capture graph builder!  hr=0x%x\0"), hr);
        return hr;
    }

	 // Get the graph's media control and media event interfaces
    hr = m_pGB.QueryInterface(&m_pMC);
	if (FAILED(hr))
	{
		Msg("Could not get Media Control Interface");
		return hr;
	};

    hr = m_pGB.QueryInterface(&m_pME);
	if (FAILED(hr))
	{
		Msg("Could not get Media Event Interface");
		return hr;
	}

    // Attach the existing filter graph to the capture graph
    hr = m_pCapture->SetFiltergraph(m_pGB);
    if (FAILED(hr))
    {
        Msg(TEXT("Failed to set capture filter graph!  hr=0x%x\0"), hr);
        return hr;
    }

	IBaseFilter *myRTSPFilter=GetFilter("RTSP Source");
	hr = m_pGB->AddFilter(myRTSPFilter, L"Video Network Proxy");
    if (FAILED(hr))
    {
        Msg(TEXT("Failed to add filter.. hr=0x%x\0"), hr);
        return hr;
    }
	DisplayFilter(myRTSPFilter);

	IBaseFilter *myFFDShowFilter=GetFilter("ffdshow MPEG-4 Video Decoder");
	hr = m_pGB->AddFilter(myFFDShowFilter, L"Video Conversion");
    if (FAILED(hr))
    {
        Msg(TEXT("Failed to add filter.. hr=0x%x\0"), hr);
        return hr;
    }

	IPin* rtspPinOut;
	IPin* ffdshowPinOut;
	IPin* ffdshowPinIn;

	GetPin(myRTSPFilter, PINDIR_OUTPUT,"Video Out", &rtspPinOut);
	GetPin(myFFDShowFilter, PINDIR_INPUT,"In", &ffdshowPinIn);
	GetPin(myFFDShowFilter, PINDIR_OUTPUT,"Out", &ffdshowPinOut);

	hr=m_pGB->Connect(rtspPinOut, ffdshowPinIn);
	if (FAILED(hr))
    {
		Msg(TEXT("DEBUG:Failed to connect pin..  hr=0x%x\0"), hr);
        return hr;
    }
	DisplayFilter(myFFDShowFilter);

	m_pFSrc = myFFDShowFilter;
	IPin *pIn = NULL;

	pIn=ffdshowPinOut;
	
	AM_MEDIA_TYPE am_media;
	//GetMediaType(rtspPinOut,&am_media);
	//GetMediaType(ffdshowPinIn,&am_media);
	GetMediaType(ffdshowPinOut,&am_media);
//	if (S_OK == GetMediaType(pIn,&am_media,srcinfo)) 
	//	pSrcConfig->SetFormat(&am_media);
	//else 
	//	m_src->show_source = TRUE;
	
	//if (m_src->show_source) 
	
	DisplayProperties(ffdshowPinOut);
	
    // Render the preview pin on the video capture filter.
    // This will create and connect any necessary transform filters.
    // We pass a pointer to the IBaseFilter interface of our DSMemoryRendere
    // video renderer, which will draw store the frames in the dedicated memory.
   // hr = m_pCapture->RenderStream (NULL, &MEDIATYPE_Video,
   //                                m_pFSrc, NULL, NULL);
    hr = m_pCapture->RenderStream (NULL, &MEDIATYPE_Video,
									m_pFSrc, NULL, pRenderer);
    if (FAILED(hr))

    {
        Msg(TEXT("Could not render the capture stream.  hr=0x%x\r\n\r\n"), hr);
        return hr;
    }
	else
	{
		//Msg(TEXT("Connected !!!"));
	}


	//if (m_src->show_filter) 
//		DisplayFilter();

    return S_OK;
}

HRESULT IntranelStreamVideo::GetMediaType(IPin *pin, AM_MEDIA_TYPE *mt) {

//	if (!srcinfo) return E_FAIL;

	CComPtr<IEnumMediaTypes> e_mt;
	pin->EnumMediaTypes(&e_mt);

	e_mt->Reset();

	AM_MEDIA_TYPE *n_mt = 0;
	
	while (S_OK == e_mt->Next(1, &n_mt,NULL)) {

		if (n_mt->majortype==MEDIATYPE_Video)
		{
			if (n_mt->subtype==MEDIASUBTYPE_RGB32)
			{
				//fprintf(stderr,"DEBUG:Media Type=Video RGB32\n");
				//Msg(TEXT("Find Media"));
				CopyMediaType(mt,n_mt);
				DeleteMediaType(n_mt);
				return S_OK;
			}
		}
		else 
			DeleteMediaType(n_mt);

		//VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*)n_mt->pbFormat;		
		//if ((pvi->bmiHeader.biWidth == srcinfo->width) && 
		//	(pvi->bmiHeader.biHeight == srcinfo->height)) {
		
	}

	return E_FAIL;
}

HRESULT IntranelStreamVideo::GetPin(IBaseFilter *pFilter,
							   PIN_DIRECTION PinDir,const char* name,
							   IPin **ppPin)

{

    IEnumPins  *pEnum;
    IPin       *pPin;

    pFilter->EnumPins(&pEnum);

    while(pEnum->Next(1, &pPin, 0) == S_OK) {

		PIN_INFO PinInfo;

        pPin->QueryPinInfo(&PinInfo);
		
		char szName[64];
		WideCharToMultiByte(CP_ACP, 0, PinInfo.achName, -1, szName, 64, 0, 0);

		//fprintf(stderr,"pin %i id=%i pin name=%s\n",PinDir,PinInfo.dir,szName);

		if ((PinInfo.dir == PinDir)&&(strcmp(szName,name)==0))
		{
			fprintf(stderr,"pin %i id=%i pin name=%s\n",PinDir,PinInfo.dir,szName);

			//HACK
			Msg(TEXT("DEBUG:Find Pin.."));
            pEnum->Release();

            *ppPin = pPin;

            return S_OK;
			
        }

        pPin->Release();

    }

    pEnum->Release();

    return E_FAIL;  

}



void IntranelStreamVideo::DisplayProperties(IPin *pIn)
{
	
    HRESULT hr;

	CComPtr<ISpecifyPropertyPages> pPages;

	hr = pIn->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPages);

	if (SUCCEEDED(hr))
	{
		PIN_INFO PinInfo;
		pIn->QueryPinInfo(&PinInfo);

		CAUUID caGUID;
		pPages->GetPages(&caGUID);

		OleCreatePropertyFrame(
			NULL,
			0,
			0,
			L"DirectShow Property Sheet",
			1,
			(IUnknown **)&(pIn), 
			caGUID.cElems,
			caGUID.pElems,
			0,
			0,
			NULL);
		CoTaskMemFree(caGUID.pElems);
		PinInfo.pFilter->Release();
	};

    pIn->Release();
	
}


void IntranelStreamVideo::DisplayFilter(IBaseFilter* filter)
{
	
	ISpecifyPropertyPages *pProp;
	HRESULT hr = filter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
		FILTER_INFO FilterInfo;
		hr = filter->QueryFilterInfo(&FilterInfo); 
		IUnknown *pFilterUnk;
		filter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		pProp->Release();
		OleCreatePropertyFrame(
			NULL,                   // Parent window
			0, 0,                   // Reserved
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);

		// Clean up.
		pFilterUnk->Release();
		//FilterInfo.pGraph->Release(); 
		CoTaskMemFree(caGUID.pElems);
	}
	else
	{
		Msg(TEXT("work pas bien"));
	}
}


HRESULT IntranelStreamVideo::FindCaptureDevice(IBaseFilter ** ppSrcFilter)
{
	
    HRESULT hr = S_OK;

    CComPtr <IMoniker> pMoniker;
    CComPtr <ICreateDevEnum> pDevEnum;
    CComPtr <IEnumMoniker> pClassEnum;

    IBaseFilter * pSrc = NULL;

    ULONG cFetched;

    if (!ppSrcFilter)
        return E_POINTER;

    try
    {
        // Create the system device enumerator
        hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                            IID_ICreateDevEnum, (void **) &pDevEnum);
        if (FAILED(hr))
        {
            Msg(TEXT("Couldn't create system device enumerator.  hr=0x%x"), hr);
            return hr;
        }

        // Create an enumerator for the video capture devices
        hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
        if (FAILED(hr))
        {
            Msg(TEXT("Couldn't create class enumerator for video input device category.  hr=0x%x"), hr);
            return hr;
        }

        // If there are no enumerators for the requested type, then 
        // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
        if (pClassEnum == NULL)
        {
            MessageBox(NULL,TEXT("No video capture device was detected.\r\n\r\n")
                    TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
                    TEXT("to be installed and working properly.  The sample will now close."),
                    TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
            return E_FAIL;
        }

        // Use the first video capture device on the device list.
        // Note that if the Next() call succeeds but there are no monikers,
        // it will return S_FALSE (which is not a failure).  Therefore, we
        // check that the return code is S_OK instead of using SUCCEEDED() macro.
        if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
        {
            // Bind Moniker to a filter object
            hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
            if (FAILED(hr))
            {
                Msg(TEXT("Couldn't bind moniker to filter object.  hr=0x%x"), hr);
                return hr;
            }
        }
        else
        {
            Msg(TEXT("Unable to access video capture device."));   
            return E_FAIL;
        }

        // Copy the found filter pointer to the output parameter.
        // Do NOT Release() the reference, since it will still be used
        // by the calling function.
        *ppSrcFilter = pSrc;

    }// try
    catch(...)
    {
        Msg(TEXT("Application encountered an unexpected error when trying to find capture device."));
        hr = E_UNEXPECTED;
	}
	
    return hr;
	
}


void IntranelStreamVideo::CheckIn()
{
	updated = 1;
}


void IntranelStreamVideo::CheckOut()
{
	updated = 0;
}

const int& IntranelStreamVideo::IsUpdated() const {
	return updated;
}



void
IntranelStreamVideo::update()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(m_mutex);

	if (m_frame->buffer && m_image.valid()) {
		m_image->setImage(this->xsize, this->ysize, 1, GL_BGRA, GL_BGRA, 
			GL_UNSIGNED_BYTE, m_frame->buffer, osg::Image::NO_DELETE, 1);
        
	}
}


///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////