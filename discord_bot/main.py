"""Discord bot that receives shotcalls from the C++ engine over TCP and plays
them as TTS audio in a voice channel using ElevenLabs.

Requires environment variables: ELEVENLABS_API_KEY, DISCORD_BOT_TOKEN
"""

import discord
from discord.ext import commands
import asyncio
import os
import socket
import threading
from elevenlabs.client import ElevenLabs
from elevenlabs import VoiceSettings
intents = discord.Intents.default()
intents.message_content = True
intents.voice_states = True
bot = commands.Bot(command_prefix="!", intents=intents)

# ElevenLabs setup
ELEVENLABS_API_KEY = os.environ["ELEVENLABS_API_KEY"]
client = ElevenLabs(api_key=ELEVENLABS_API_KEY)

# Popular voice IDs
VOICES = {
    "rachel": "EXAVITQu4vr4xnSDxMaL",
    "domi": "AZnzlk1XvdvUeBnXmlld",
    "bella": "MF3mGyEYCl7XYWbV9V6O",
    "antoni": "ErXwobaYiN019PkySvjV",
    "adam": "21m00Tcm4TlvDq8ikWAM",
    "sam": "yoZ06aMxZJJ28mfd3POQ",
    "josh": "TxGEqnHWrfWFTfGW9XjX",
    "arnold": "VR6AewLTigWG4xSOukaG",
    "callum": "N2lVS1w4EtoT3dr4eOWO",
    "charlie": "IKne3meq5aSn9XLyUdCD",
}

current_voice = "rachel"


# Function to speak text from C++ or commands
async def speak_text(text, voice_client):
    """Generate and play TTS audio"""
    try:
        # Generate TTS audio with ElevenLabs
        audio_generator = client.generate(
            text=text,
            voice=VOICES[current_voice],
            model="eleven_turbo_v2",  # Faster model for real-time
            voice_settings=VoiceSettings(
                stability=0.5, similarity_boost=0.75, style=0.0, use_speaker_boost=True),
        )

        # Collect audio chunks
        audio_data = b"".join(audio_generator)

        # Save temporarily
        filename = f"tts_{hash(text)}.mp3"
        with open(filename, "wb") as f:
            f.write(audio_data)

        # Wait if already playing something
        while voice_client.is_playing():
            await asyncio.sleep(0.1)

        voice_client.play(discord.FFmpegPCMAudio(filename))

        # Wait for playback to finish, then delete file
        while voice_client.is_playing():
            await asyncio.sleep(0.1)

        os.remove(filename)

    except Exception as e:
        print(f"Error in speak_text: {e}")
        if os.path.exists(filename):
            os.remove(filename)


# Socket server to receive messages from C++
def socket_server():
    """Listen for messages from C++ application"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("localhost", 9999))
    server.listen(5)
    print("Socket server listening on localhost:9999...")

    while True:
        try:
            conn, addr = server.accept()
            data = conn.recv(4096).decode("utf-8")
            if data:
                print(f"Received from C++: {data}")
                # Add message to bot's event loop
                asyncio.run_coroutine_threadsafe(
                    speak_from_cpp(data), bot.loop)
            conn.close()
        except Exception as e:
            print(f"Socket error: {e}")


async def speak_from_cpp(text):
    """Speak text received from C++ application"""
    # Find first connected voice client
    for vc in bot.voice_clients:
        if vc.is_connected():
            await speak_text(text, vc)
            break
    else:
        print("Warning: Bot not connected to any voice channel")


@bot.event
async def on_ready():
    print(f"{bot.user} is now running!")
    print(f"Current voice: {current_voice}")
    print(f'Available voices: {", ".join(VOICES.keys())}')

    # Start socket server in separate thread
    threading.Thread(target=socket_server, daemon=True).start()


@bot.command()
async def join(ctx):
    """Join the voice channel you're in"""
    if ctx.author.voice:
        channel = ctx.author.voice.channel
        await channel.connect()
        await ctx.send(f"Joined {channel.name}!")
    else:
        await ctx.send("You need to be in a voice channel!")


@bot.command()
async def leave(ctx):
    """Leave the voice channel"""
    if ctx.voice_client:
        await ctx.voice_client.disconnect()
        await ctx.send("Left the voice channel!")
    else:
        await ctx.send("I'm not in a voice channel!")


@bot.command()
async def say(ctx, *, text: str):
    """Make the bot speak text in voice channel"""
    if not ctx.voice_client:
        await ctx.send("I'm not in a voice channel! Use !join first.")
        return

    if len(text) > 500:
        await ctx.send("Text too long! Max 500 characters.")
        return

    try:
        await ctx.send(f"Speaking: *{text[:50]}{'...' if len(text) > 50 else ''}*")
        await speak_text(text, ctx.voice_client)

    except Exception as e:
        await ctx.send(f"Error: {str(e)}")


@bot.command()
async def voice(ctx, voice_name: str = None):
    """Change the voice. Usage: !voice [voice_name] or !voice to list"""
    global current_voice

    if voice_name is None:
        voice_list = "\n".join(
            [f"**{name}** - {get_voice_description(name)}" for name in VOICES.keys()])
        await ctx.send(f"**Current voice:** {current_voice}\n\n**Available voices:**\n{voice_list}")
        return

    voice_name = voice_name.lower()
    if voice_name in VOICES:
        current_voice = voice_name
        await ctx.send(f"Voice changed to: **{current_voice}**")
    else:
        await ctx.send("Invalid voice! Use `!voice` to see available voices.")


def get_voice_description(voice_name):
    """Get description for a voice"""
    descriptions = {
        "rachel": "Calm, young female",
        "domi": "Strong, confident female",
        "bella": "Soft, friendly female",
        "antoni": "Well-rounded male",
        "adam": "Deep male narrator",
        "sam": "Raspy young male",
        "josh": "Conversational male",
        "arnold": "Crisp male narrator",
        "callum": "Hoarse middle-aged male",
        "charlie": "Casual Australian male",
    }
    return descriptions.get(voice_name, "Unknown")


# Run the bot
if __name__ == "__main__":
    bot.run(os.environ["DISCORD_BOT_TOKEN"])
