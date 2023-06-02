#pragma once

class Animation {
private:
    std::vector<sf::Texture> frames;
    bool loop;
    bool isPlaying;
    int current_frame;
    int num_frames;
    float frame_duration;
    float elapsed_time;

public:
    Animation(float frameDuration, int numFrames, bool loop = true) : 
        frame_duration(frameDuration), num_frames(numFrames), loop(loop), isPlaying(false), current_frame(0), elapsed_time(0.0f) {}
    void addFrame(const sf::Texture& texture) { frames.push_back(texture); }
    const sf::Texture& getCurrentFrame() const { return frames[current_frame]; }
    void play() { isPlaying = true; }
    void load(std::string path);
    void update(float deltaTime);
    void stop();
};

void Animation::load(std::string path)
{
    // Load textures for animation
    for (int i = 0; i < num_frames; i++)
    {
        sf::Texture texture;
        texture.loadFromFile(path + std::to_string(i) + ".png");
        frames.push_back(texture);
    }
}

void Animation::update(float deltaTime) 
{
    // Update texture based on current frame calculated from elapsed time
    if (!isPlaying) return;
    elapsed_time += deltaTime;

    if (elapsed_time >= frame_duration) 
    {
        elapsed_time -= frame_duration;
        current_frame++;

        if (current_frame == frames.size()) {
            if (loop) current_frame = 0;
            else {
                isPlaying = false;
                current_frame = frames.size() - 1;
            }
        }
    }
}

void Animation::stop() 
{
    // Stop and reset animation
    isPlaying = false;
    current_frame = 0;
    elapsed_time = 0.0f;
}