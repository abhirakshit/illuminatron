# Illuminatron: An interactive platform for visualizing, sonifying and transforming our inner worlds

A tool for individuals & clinicians to improve mental health & wellbeing using psychological profiles with realtime audio & visual feedback from biosensors to manage psychological challenges and better understand their lives

## Project Details
Visualization has facilitated the intuitive grasp of complex systems such as weather and stock markets. However, the power of visualization in a mental health context has yet to be explored extensively in clinical settings. A means of depicting and playing with representations of our inner worlds could facilitate self-understanding and emotional regulation. This could potentially serve as a therapeutic intervention in a clinical context. 
Illuminatron is a 3D workspace that introduces an intuitive visual language to illustrate cognitive processes and personality variables. It thereby facilitates understanding and improved management of peoples’  internal struggles and challenges. Psychological traits and events are depicted using properties of light; for example., reflecting on an idea is depicted as reflection of a beam of light within a sphere. The universal resonance of this "metaphor of light" is evidenced by how in all languages, psychological events are expressed using words that refer also to light (e.g., ray of hope, creative spark, beam with enthusiasm, moment of illumination, brilliant idea,…). Users create visual depictions of their current mental state, and who they aspire to be, and then plot a journey from present self to desired future self. 

The representational system has three components: optics, electromagnetism, and quantum mechanics. Here we give only a sneak peek into rudimentary elements of the optical component. The mind or ego is portrayed as multilayered spherical shells. This structure contains a person's creative life force, chi, or Inner Light (IL), which is portrayed as a ‘background noise’ of flickering light with a unique spectrum. Conscious thoughts are depicted as beams of light within the sphere. The importance, content, and clarity of a thought are represented by its intensity, frequency, and dispersion, respectively. Bending the truth is depicted as refraction (bending) of light. Other spheres can be created to represent other people in your life, and how you interact with them. Individual differences are depicted visually with properties such as opacity/transparency of shell for trait personality openness, and degree of life force is depicted by intensity of inner light. 


Further details about the project and the representational system can be found [here](https://gabora-psych.ok.ubc.ca/IN.html) along with a short [intro video](https://www.youtube.com/watch?v=7-WLlZg9keM) of the preliminary work. The website and the video both are being updated and we are working on moving them to their own domain. 

# Instructions

## Dependencies

The versions listed are the only ones **known** to work.

 - Unreal Engine 4.22 - 4.27
     - UE Plugin: Electronic Nodes (optional?)
	    - This plugin improves blueprint formatting to help keep things tidy, the blueprints should work without the plugin but this has not been verified.
 - Visual Studio 2017 or later
     - Individual Components
	    - MSVC v141 - VS 2017
	    - Windows 10 SDK (possibly optional, 10.0.18362 or 10.0.19041 or possibly any will do; please update this doc if you learn more)
     - Workloads
	    - .NET desktop development
        - Desktop development with C++


## Getting Started with Unreal

Unreal Engine provides a powerful editor that allows us to create blueprints for a lot of things, but when performance is a concern we generally write C++ code. This C++ code integrates itself directly into the editor, but as with all C++ code, binaries must be compiled.

In other words, in order to run our project in the Unreal Engine Editor [allowing us to create blueprints or otherwise utilize our C++ code] we need to build a binary for the Editor to load. This binary is known as a module.

Follow these instructions to just get it working:

1. Navigate to the repo directory
2. Right click the .uproject file
3. Select generate project files
4. Open the new .sln file inside Visual Studio
5. Build the solution or project (Ctrl+Shift+B / Ctrl+B)
6. Click `|> Local Windows Debugger` or open the .uproject file in Unreal Engine (running the editor in the Visual Studio debugger is helpful, but it may come with performance issues on some machines)


Now that you can open the UE project, here are some extended instructions to give a sense of the worflow:

1. Create blueprints creating UCLASSES exported from C++
2. Test your blueprint in the Editor's debugger
3. Change the C++ code
4. To build and implement C++ changes:
	- If running **with** VS debugger: you need to click `Compile` inside the UE Editor window.
	- If running **without** VS debugger: you can simply rebuild from inside Visual Studio. 
	- **Note:** the hot reload inside VS may work, but may also give errors