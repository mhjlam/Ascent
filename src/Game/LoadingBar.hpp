#pragma once

#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>

/**
 * Modern loading bar implementation using Ogre overlays
 * Creates a simple progress bar overlay for resource loading
 */
class LoadingBar : public Ogre::ResourceGroupListener {
public:
    LoadingBar();
    virtual ~LoadingBar();

    void start(Ogre::RenderWindow* window);
    void finish();
    
    // Show a simple loading screen for level transitions
    void show_level_loading(Ogre::RenderWindow* window, const std::string& message = "Loading Level...");
    void hide_level_loading();

protected:
    // ResourceGroupListener callbacks
    void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount) override;
    void scriptParseStarted(const Ogre::String& scriptName, bool &skipThisScript) override;
    void scriptParseEnded(const Ogre::String& scriptName, bool skipped) override;
    void resourceGroupScriptingEnded(const Ogre::String& groupName) override;
    void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount) override;
    void resourceLoadStarted(const Ogre::ResourcePtr& resource) override;
    void resourceLoadEnded() override;
    void resourceGroupLoadEnded(const Ogre::String& groupName) override;

private:
    void create_overlay();
    void update_progress(float progress);

    Ogre::RenderWindow* render_window_;
    Ogre::Overlay* overlay_;
    Ogre::OverlayContainer* panel_;
    Ogre::OverlayContainer* progress_bar_container_;
    Ogre::OverlayElement* progress_bar_;
    Ogre::OverlayElement* loading_text_;
    
    size_t num_resources_;
    size_t resources_loaded_;
    float progress_;
};
