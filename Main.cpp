/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include "theoraApplicationPCH.h"
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>
#include <theoraplayer\TheoraVideoManager.h>
#include <theoraplayer\TheoraVideoFrame.h>

#pragma comment(lib,"libtheoraplayer.lib")

// Use the following line to initialize a plugin that is statically linked.
// Note that only Windows platform links plugins dynamically (on Windows you can comment out this line).
VIMPORT IVisPlugin_cl* GetEnginePlugin_theoraPlugin();

class theoraApplicationClass : public VAppImpl
{
public:
  theoraApplicationClass() {}
  virtual ~theoraApplicationClass() {}

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE;
  virtual void PreloadPlugins() HKV_OVERRIDE;

  virtual void Init() HKV_OVERRIDE;
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;
};

VAPP_IMPLEMENT_SAMPLE(theoraApplicationClass);

void theoraApplicationClass::SetupAppConfig(VisAppConfig_cl& config)
{
  // Set custom file system root name ("havok_sdk" by default)
  config.m_sFileSystemRootName = "template_root";

  // Set the initial starting position of our game window and other properties
  // if not in fullscreen. This is only relevant on windows
  config.m_videoConfig.m_iXRes = 1280; // Set the Window size X if not in fullscreen.
  config.m_videoConfig.m_iYRes = 720;  // Set the Window size Y if not in fullscreen.
  config.m_videoConfig.m_iXPos = 50;   // Set the Window position X if not in fullscreen.
  config.m_videoConfig.m_iYPos = 50;   // Set the Window position Y if not in fullscreen.

  // Name to be displayed in the windows title bar.
  config.m_videoConfig.m_szWindowTitle = "theora";

  config.m_videoConfig.m_bWaitVRetrace = true;

  // Fullscreen mode with current desktop resolution
  
#if defined(WIN32)
  /*
  DEVMODEA deviceMode;
  deviceMode = Vision::Video.GetAdapterMode(config.m_videoConfig.m_iAdapter);
  config.m_videoConfig.m_iXRes = deviceMode.dmPelsWidth;
  config.m_videoConfig.m_iYRes = deviceMode.dmPelsHeight;
  config.m_videoConfig.m_bFullScreen = true;
  */
#endif
  
}

void theoraApplicationClass::PreloadPlugins()
{
  // Use the following line to load a plugin. Remember that, except on Windows platform, in addition
  // you still need to statically link your plugin library (e.g. on mobile platforms) through project
  // Properties, Linker, Additional Dependencies.
  VISION_PLUGIN_ENSURE_LOADED(theoraPlugin);
}

//---------------------------------------------------------------------------------------------------------
// Init function. Here we trigger loading our scene
//---------------------------------------------------------------------------------------------------------

VisScreenMask_cl* m_spVideoScreenMask;
VTextureObject* m_spVideoTexture;
TheoraVideoManager mgr;
TheoraVideoClip *clip;

void theoraApplicationClass::Init()
{
  // Set filename and paths to our stand alone version.
  // Note: "/Data/Vision/Base" is always added by the sample framework
  VisAppLoadSettings settings("Scenes/Default.vscene");
  settings.m_customSearchPaths.Append(":template_root/Assets");
  LoadScene(settings);

  clip = mgr.createVideoClip("c:/Projects/theora/libtheoraplayer_sdk_1.0.0/demos/media/bunny.ogg", TH_BGR);
  clip->play();
  m_spVideoScreenMask = new VisScreenMask_cl();
m_spVideoScreenMask->SetOrder(5);
m_spVideoScreenMask->SetFiltering(FALSE);
m_spVideoScreenMask->SetPos(0,0);

m_spVideoTexture = Vision::TextureManager.Create2DTextureObject("<VideoTex>", clip->getWidth(), clip->getHeight(), 1 /*num mipmaps*/, VTextureLoader::DEFAULT_TEXTURE_FORMAT_24BPP);
m_spVideoScreenMask->SetTextureObject(m_spVideoTexture);

// fullscreen mode setup
// m_spVideoScreenMask->SetTargetSize(Vision::Video.GetXRes(), Vision::Video.GetYRes());
}

//---------------------------------------------------------------------------------------------------------
// Gets called after the scene has been loaded
//---------------------------------------------------------------------------------------------------------
void theoraApplicationClass::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // Define some help text
  VArray<const char*> help;
  help.Append("How to use this demo...");
  help.Append("");
  RegisterAppModule(new VHelp(help));

  // Create a mouse controlled camera (set above the ground so that we can see the ground)
  Vision::Game.CreateEntity("VisMouseCamera_cl", hkvVec3(0.0f, 0.0f, 170.0f));

    VisBaseEntity_cl *e = Vision::Game.SearchEntity(NULL);
  e->GetMesh()->GetSurface(0)->SetBaseTexture(m_spVideoTexture);
  // Add other initial game code here
  // [...]
}

//---------------------------------------------------------------------------------------------------------
// Main Loop of the application until we quit
//---------------------------------------------------------------------------------------------------------
bool theoraApplicationClass::Run()
{
	
	unsigned char *pVideoFrameBuffer=NULL;
	TheoraVideoFrame *pFrame = clip->getNextFrame();
if (pFrame)
{
  pVideoFrameBuffer = pFrame->getBuffer(); //here I assume that the video is in the right format (32BPP)
  clip->popFrame();

//update the texture and the screenmask every frame (after you received the new frame from the decoder)
m_spVideoTexture->UpdateRect(0, //mipmap
                             0,0, //x,y
                             0,0, //w,h
							 clip->getStride()*3, //Stride 32BPP = 4 byte
                             pVideoFrameBuffer,
                             V_TEXTURE_LOCKFLAG_DISCARDABLE);
}
 mgr.update( Vision::GetTimer()->GetTimeDifference() );

 return true;
}

void theoraApplicationClass::DeInit()
{
  // De-Initialization
  // [...]
}

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
