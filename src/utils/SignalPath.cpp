#include "utils/SignalPath.h"

juce::Point<float> CachedPath::getPoint (float t) const
{
    if (samples.size() < 2)
        return {};

    t = juce::jlimit (0.0f, 1.0f, t);

    float pos = t * (samples.size() - 1);

    int i0 = (int) pos;
    int i1 = std::min (i0 + 1, (int) samples.size() - 1);

    float frac = pos - i0;

    return samples[i0]
        + (samples[i1] - samples[i0]) * frac;
    // return path.getPointAlongPath(t * length);
}

void CachedPath::rebuildCache ()
{
    length = path.getLength();

    int numSamples = std::max(
        2,
        (int) std::ceil(length / SIGNALPATH_SAMPLE_SPACING)
    );
    samples.clear();
    samples.reserve(numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        float t = (float) i / (numSamples - 1);

        samples.push_back(
            path.getPointAlongPath(t * length));
    }
}
void CachedPath::updateCachedPath (float intensity)
{
    intensity = juce::jlimit (-1.0f, 1.0f, intensity);
    phase += intensity * SIGNALPATH_SPEED / length;
    phase = std::fmod(phase, 1.0f);
}

void SignalPath::createSignalPath(std::vector<juce::Path>& paths, SignalPath* ref, bool mode){
    setRef(ref);
    setMode(mode);
    for (auto& p : paths){
        addPath(p);
    }
    shufflePhase();
}
void SignalPath::createSignalPath(juce::Path& path, SignalPath* ref, bool mode){
    setRef(ref);
    setMode(mode);
    addPath(path);
}
void SignalPath::addPath(juce::Path& path, float rangeMin, float rangeMax)
{
    jassert (rangeMin <= rangeMax);
    auto s = CachedPath{path};
    s.rangeMin = juce::jlimit(0.0f, 1.0f, rangeMin);
    s.rangeMax = juce::jlimit(0.0f, 1.0f, rangeMax);
    s.rebuildCache();
    signalPaths.push_back(s);
}
void SignalPath::setRef(SignalPath* ref){
    refNode = ref;
}
void SignalPath::setPolarity(bool pol ){
    polarity = pol;
}
void SignalPath::setDirection(bool dir ){
    direction = dir;
}
void SignalPath::setMode(const int mode ){
    setPolarity(mode%2 == 0 ? POLARITY_FORWARD : POLARITY_BACKWARD);
    setDirection(mode < 2 ? DIRECTION_FORWARD : DIRECTION_BACKWARD);
}
void SignalPath::shufflePhase(){
    auto n = signalPaths.size();
    for( int i =0; i < n; i++){
        signalPaths[i].phase = (float)i/ (float)n;
    }
}

void SignalPath::updateSignalPath (float intensity, float voltage) {
    if (refNode != nullptr){
        refVoltage = refNode->getRefVoltage() - (refNode->direction ? -1.0f : 1.0f) * refNode->getDeltaVoltage();
        deltaVoltage = voltage;
    }
    else{
        refVoltage =  voltage;
        deltaVoltage = 0.0F;
    }

    for (auto& cachedPath : signalPaths)
        cachedPath.updateCachedPath(intensity);
}
void SignalPath::draw (juce::Graphics& g)
{
    if (signalPaths.size() == 0)
        return;

        

    auto pt = polarity ? 1.0f : -1.0f;
    float colort; 
    auto deltat = deltaVoltage/SIGNAL_PATH_MAX_VOLTAGE;
    auto reft = refVoltage/SIGNAL_PATH_MAX_VOLTAGE;
    float beadSize = SIGNALPATH_BEAD_SIZE ;

    for (auto& sigPath : signalPaths){
        if (sigPath.samples.size() < 2)
            return;

        int numBeads = juce::jmax (1, (int) (sigPath.length / SIGNALPATH_BEAD_SPACING));
        float spacing = 1.0f / numBeads;
        auto rangeMin = sigPath.rangeMin;
        auto rangeMax = sigPath.rangeMax;
        float tRange = rangeMax - rangeMin;

        for (int i = 0; i < numBeads; ++i)
        {
            float offset = i * spacing;
            float t = offset + sigPath.phase;
            t -= std::floor(t);
            t = direction ? t : 1.0f - t; 

            juce::Point<float> p = sigPath.getPoint (t);

            juce::Path bead;
            bead.addEllipse (p.x - beadSize * 0.5f,
                            p.y - beadSize * 0.5f,
                            beadSize,
                            beadSize);

            t = polarity ? t : 1.0f - t; 
            colort = reft - (direction ? -1.0f : 1.0f) * deltat *( t *tRange + rangeMin);

            auto color = getVoltageColourGradient(colort);
            drawGlowPath(g,bead,color, 0.2f);
            drawSolidCorePath(g,bead, false, color);
        }

    }
}