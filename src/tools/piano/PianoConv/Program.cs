using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using NAudio.Wave;

namespace PianoConv
{
    class Program
    {

        static void AiffTrimmer(string src, string dst)
        {
            var reader = new NAudio.Wave.AiffFileReader(src);
            float[] samples = new float[reader.SampleCount];
            reader.ToSampleProvider().Read(samples, 0, samples.Length / 4 * 4);

            int iStart = -1;
            for (int i = 0; i < samples.Length; i++)
            {
                if ((samples[i] > 0.002) && (iStart < 0))
                    iStart = i;
            }

            int startEnvLen = reader.WaveFormat.SampleRate/30;
            iStart = Math.Max(0, iStart - startEnvLen);

            int iEnd = samples.Length;
            var comp = Math.Max(Math.Abs(samples.Skip(iStart).Take(80000).Max()),
                Math.Abs(samples.Skip(iStart).Take(80000).Min()));

            for (int i = iStart, ival = 80000; i < samples.Length; i += ival)
            {
                var max = Math.Max(Math.Abs(samples.Skip(i).Take(ival).Max()),
                    Math.Abs(samples.Skip(i).Take(ival).Min()));
                var weight = max/comp;

                if ((weight < 0.02) || (max < 0.001))
                {
                    iEnd = i;
                    break;
                }
            }

            samples = samples.Skip(iStart/4*4).Take((iEnd - iStart)/4*4).ToArray();

            // create start envelope
            for (int i = 0; i < startEnvLen; i++)
            {
                float step = 1.0f/startEnvLen;

                samples[i] *= (step*i);
            }

            // create end envelope
            for (int i = iEnd - startEnvLen, x = startEnvLen; i < samples.Length; i++, x--)
            {
                float step = 1.0f / startEnvLen;

                samples[i] *= (step * x);
            }

            // write samples to file
            var writer = new WaveFileWriter(dst, reader.WaveFormat);
            writer.WriteSamples(samples, 0, samples.Length);
            writer.Flush();
            writer.Dispose();
        }

        static void Main(string[] args)
        {
            foreach (var httpPath_ in File.ReadAllLines(@"C:\Users\natalie\Documents\piano\list.txt"))
            {
                var httpPath = "http://theremin.music.uiowa.edu/" + httpPath_;
                var targetFile = Path.GetFileName(httpPath_);
                var trimPath = @"I:\projects\Multimedia\iSynth\tools\piano\aiff-trimmed\" +
                               Path.GetFileNameWithoutExtension(targetFile) + ".wav";
                var aiffPath = @"I:\projects\Multimedia\iSynth\tools\piano\aiff\" + targetFile;

                if (!File.Exists(aiffPath))
                {
                    Console.WriteLine("Downloading: " + Path.GetFileName(targetFile));

                    var request = WebRequest.CreateHttp(httpPath);
                    using (var respone = request.GetResponse().GetResponseStream())
                    {
                        var mem = new MemoryStream();
                        respone.CopyTo(mem);

                        File.WriteAllBytes(aiffPath, mem.ToArray());
                    }
                }

                if (!File.Exists(trimPath))
                {
                    AiffTrimmer(aiffPath, trimPath);
                }
            }

            Console.Write("Download complete!");
        }
    }
}
