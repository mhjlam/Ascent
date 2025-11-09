#include "Game/LoadingBar.hpp"

#include <iostream>


LoadingBar::LoadingBar()
: render_window_(nullptr)
, overlay_(nullptr)
, panel_(nullptr)
, progress_bar_container_(nullptr)
, progress_bar_(nullptr)
, loading_text_(nullptr)
, num_resources_(0)
, resources_loaded_(0)
, progress_(0.0f) {}


LoadingBar::~LoadingBar() {
    if (overlay_) {
        Ogre::OverlayManager::getSingleton().destroy(overlay_);
    }
}


void LoadingBar::create_overlay() {
    Ogre::OverlayManager& overlay_mgr = Ogre::OverlayManager::getSingleton();
    
    // Check if overlay already exists
    if (overlay_) {
        return;
    }
    
    // Check if elements already exist and destroy them first
    if (overlay_mgr.hasOverlayElement("LoadingPanel")) {
        overlay_mgr.destroyOverlayElement("LoadingPanel");
    }
    if (overlay_mgr.hasOverlayElement("LoadingText")) {
        overlay_mgr.destroyOverlayElement("LoadingText");
    }
    if (overlay_mgr.hasOverlayElement("ProgressBarContainer")) {
        overlay_mgr.destroyOverlayElement("ProgressBarContainer");
    }
    if (overlay_mgr.hasOverlayElement("ProgressBar")) {
        overlay_mgr.destroyOverlayElement("ProgressBar");
    }
    
    // Create overlay
    overlay_ = overlay_mgr.create("LoadingOverlay");
    
    // Create main panel (semi-transparent background)
    panel_ = static_cast<Ogre::OverlayContainer*>(
        overlay_mgr.createOverlayElement("Panel", "LoadingPanel"));
    panel_->setMetricsMode(Ogre::GMM_RELATIVE);
    panel_->setPosition(0, 0);
    panel_->setDimensions(1.0, 1.0);
    // Note: No material needed for transparent background
    
    // Create loading text (will set font later after resources load)
    loading_text_ = overlay_mgr.createOverlayElement("TextArea", "LoadingText");
    loading_text_->setMetricsMode(Ogre::GMM_RELATIVE);
    loading_text_->setPosition(0.35f, 0.45f);
    loading_text_->setDimensions(0.3f, 0.05f);
    // Note: Font is set after initial resource group loads
    loading_text_->setParameter("char_height", "0.04");
    loading_text_->setParameter("colour_top", "1 1 1");
    loading_text_->setParameter("colour_bottom", "1 1 1");
    loading_text_->setCaption("Loading...");
    
    // Create progress bar container (border/background)
    progress_bar_container_ = static_cast<Ogre::OverlayContainer*>(
        overlay_mgr.createOverlayElement("Panel", "ProgressBarContainer"));
    progress_bar_container_->setMetricsMode(Ogre::GMM_RELATIVE);
    progress_bar_container_->setPosition(0.25f, 0.55f);
    progress_bar_container_->setDimensions(0.5f, 0.03f);
    // Set a solid color using colour parameter instead of material
    progress_bar_container_->setParameter("colour", "0.2 0.2 0.2");
    
    // Create progress bar fill
    progress_bar_ = overlay_mgr.createOverlayElement("Panel", "ProgressBar");
    progress_bar_->setMetricsMode(Ogre::GMM_RELATIVE);
    progress_bar_->setPosition(0.01f, 0.1f);
    progress_bar_->setDimensions(0.0f, 0.8f); // Start at 0 width
    progress_bar_->setParameter("colour", "0 1 0");  // Green color
    
    // Build hierarchy
    progress_bar_container_->addChild(progress_bar_);
    panel_->addChild(loading_text_);
    panel_->addChild(progress_bar_container_);
    overlay_->add2D(panel_);
    
    overlay_->show();
}

void LoadingBar::update_progress(float progress) {
    progress_ = progress;
    if (progress_bar_) {
        // Scale progress bar width (subtract border space)
        progress_bar_->setWidth(progress * 0.996f);
        
        // Update text
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Loading... %.0f%%", progress * 100.0f);
        if (loading_text_) {
            loading_text_->setCaption(buffer);
        }
        
        // Force update
        if (render_window_) {
            Ogre::Root::getSingleton().renderOneFrame();
        }
    }
}

void LoadingBar::start(Ogre::RenderWindow* window) {
    render_window_ = window;
    create_overlay();
    
    // Register as listener
    Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);
}

void LoadingBar::finish() {
    // Unregister listener
    Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);
    
    // Hide and destroy overlay
    if (overlay_) {
        std::cout << "LoadingBar::finish() - Hiding LoadingOverlay" << std::endl;
        overlay_->hide();
        std::cout << "LoadingOverlay visible after hide: " << (overlay_->isVisible() ? "YES" : "NO") << std::endl;
    }
}

void LoadingBar::show_level_loading(Ogre::RenderWindow* window, const std::string& message) {
    render_window_ = window;
    
    // Create or reuse overlay
    if (!overlay_) {
        create_overlay();
    }
    
    // Update text
    if (loading_text_) {
        loading_text_->setCaption(message);
    }
    
    // Show overlay
    overlay_->show();
    
    // Render one frame to display the loading screen
    if (render_window_) {
        Ogre::Root::getSingleton().renderOneFrame();
    }
}

void LoadingBar::hide_level_loading() {
    if (overlay_) {
        overlay_->hide();
    }
}

void LoadingBar::resourceGroupScriptingStarted([[maybe_unused]] const Ogre::String& groupName, size_t scriptCount) {
    num_resources_ = scriptCount;
    resources_loaded_ = 0;
    
    // Try to set font now that resources are being loaded
    try {
        if (loading_text_) {
            loading_text_->setParameter("font_name", "BlueHighway-12");
        }
    } catch (...) {
        // Font not available yet, will remain without font
    }
}

void LoadingBar::scriptParseStarted([[maybe_unused]] const Ogre::String& scriptName, [[maybe_unused]] bool &skipThisScript) {
    // Optional: Update with script name
}

void LoadingBar::scriptParseEnded([[maybe_unused]] const Ogre::String& scriptName, bool skipped) {
    if (!skipped) {
        resources_loaded_++;
        if (num_resources_ > 0) {
            update_progress(static_cast<float>(resources_loaded_) / static_cast<float>(num_resources_) * 0.3f);
        }
    }
}

void LoadingBar::resourceGroupScriptingEnded([[maybe_unused]] const Ogre::String& groupName) {
    // Scripting phase complete
}

void LoadingBar::resourceGroupLoadStarted([[maybe_unused]] const Ogre::String& groupName, size_t resourceCount) {
    num_resources_ = resourceCount;
    resources_loaded_ = 0;
}

void LoadingBar::resourceLoadStarted([[maybe_unused]] const Ogre::ResourcePtr& resource) {
    // Optional: Update with resource name
}

void LoadingBar::resourceLoadEnded() {
    resources_loaded_++;
    if (num_resources_ > 0) {
        update_progress(0.3f + static_cast<float>(resources_loaded_) / static_cast<float>(num_resources_) * 0.7f);
    }
}

void LoadingBar::resourceGroupLoadEnded([[maybe_unused]] const Ogre::String& groupName) {
    update_progress(1.0f);
}
