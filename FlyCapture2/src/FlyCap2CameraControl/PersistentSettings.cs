//=============================================================================
// Copyright © 2018 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Xml.Serialization;

namespace FlyCap2CameraControl
{
    [Serializable]
    public class PersistentSettings
    {
        private readonly Mutex _fileIoGuard;
        private readonly float _version = 1.0f;
        private bool _enableSettings = false;
        private const string K_SettingsFileName = "FlyCap2CameraControl.xml";
        private static PersistentSettings instance;
        private CustomVideoModeSettings _customVideoModeSettings;
        private Brightness _brightnessPersistentSettings;
        private FrameRate _frameRatePersistentSettings;

        [XmlAttribute("XMLVersion")]
        public float Version
        {
            get
            {
                return _version;
            }
            set
            {

            }
        }

        [XmlElement("EnableSettings")]
        public bool EnableSettings
        {
            get
            {
                return _enableSettings;
            }
            set
            {
                _enableSettings = value;
            }

        }

        [XmlElement("CustomVideo")]
        public CustomVideoModeSettings CustomVideoModePersistentSettings
        {
            get
            {
                return _customVideoModeSettings;
            }
            set
            {
                _customVideoModeSettings = value;
            }
        }

        [XmlElement("Brightness")]
        public Brightness BrightnessPersistentSettings
        {
            get
            {
                return _brightnessPersistentSettings;
            }
            set
            {
                _brightnessPersistentSettings = value;
            }
        }

        [XmlElement("FrameRate")]
        public FrameRate FrameRatePersistentSettings
        {
            get
            {
                return _frameRatePersistentSettings;
            }
            set
            {
                _frameRatePersistentSettings = value;
            }
        }

        private PersistentSettings()
        {
            _fileIoGuard = new Mutex();
            CustomVideoModePersistentSettings = new CustomVideoModeSettings();
            BrightnessPersistentSettings = new Brightness();
            FrameRatePersistentSettings = new FrameRate();
        }

        public static PersistentSettings Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new PersistentSettings();
                }

                return instance;
            }
        }

        private static string GetSettingsFilePath()
        {
            return K_SettingsFileName;
        }

        public void LoadPersistentSettings()
        {
            _fileIoGuard.WaitOne();

            FileStream fs = null;

            try
            {
                PersistentSettings settings = null;

                // Open file to read the data from
                fs = new FileStream(GetSettingsFilePath(), FileMode.Open);

                // Create an XmlSerializer object to perform the deserialization
                XmlSerializer xs = new XmlSerializer(typeof(PersistentSettings));

                // Use the XmlSerializer object to deserialize the PersistentSettings class
                settings = (PersistentSettings) xs.Deserialize(fs);

                // Copy settings
                CopySettings(settings);
            }
            catch (FileNotFoundException)
            {
                this.EnableSettings = false;
                Debug.WriteLine(String.Format("{0} is not found.  Default settings will be used.", GetSettingsFilePath())); 
            }
            catch (Exception ex)
            {
                this.EnableSettings = false;
                Debug.WriteLine(String.Format("Warning: Unable to parse {0}. Default settings will be used.", GetSettingsFilePath()));
            }
            finally
            {
                if (fs != null)
                {
                    // Close the file
                    fs.Close();
                }

                _fileIoGuard.ReleaseMutex();
            }
        }

        public void SavePersistentSettings()
        {
            _fileIoGuard.WaitOne();

            FileStream fs = null;

            try
            {
                // Create file to save the data to
                fs = new FileStream(GetSettingsFilePath(), FileMode.Create);

                // Create an XmlSerializer object to perform the serialization
                XmlSerializer xs = new XmlSerializer(typeof(PersistentSettings));

                // Use the XmlSerializer object to serialize PersistentSettings class
                xs.Serialize(fs, this);
            }
            finally
            {
                if (fs != null)
                {
                    // Close the file
                    fs.Close();
                }

                _fileIoGuard.ReleaseMutex();
            }
        }

        private void CopySettings(PersistentSettings settings)
        {
            this.EnableSettings = settings.EnableSettings;
            this.CustomVideoModePersistentSettings = settings.CustomVideoModePersistentSettings;
            this.BrightnessPersistentSettings = settings.BrightnessPersistentSettings;
            this.FrameRatePersistentSettings = settings.FrameRatePersistentSettings;
        }

        [Serializable]
        public class CustomVideoModeSettings
        {
            private bool _useMaxImageSize = true;

            [XmlElement]
            public bool UseMaxImageSize
            {
                get
                {
                    return _useMaxImageSize;
                }
                set
                {
                    _useMaxImageSize = value;
                }
            }
        }

        [Serializable]
        public class Brightness
        {
            private bool _autoMode = true;
            private bool _absControl = true;
            private float _value = 255f;

            [XmlElement]
            public bool AutoMode
            {
                get
                {
                    return _autoMode;
                }
                set
                {
                    _autoMode = value;
                }
            }

            [XmlElement]
            public bool AbsControl
            {
                get
                {
                    return _absControl;
                }
                set
                {
                    _absControl = value;
                }
            }

            [XmlElement]
            public float Value
            {
                get
                {
                    return _value;
                }
                set
                {
                    _value = value;
                }
            }
        }

        [Serializable]
        public class FrameRate
        {
            private bool _autoMode = true;
            private bool _absControl = true;
            private float _value = 150f;

            [XmlElement]
            public bool AutoMode
            {
                get
                {
                    return _autoMode;
                }
                set
                {
                    _autoMode = value;
                }
            }

            [XmlElement]
            public bool AbsControl
            {
                get
                {
                    return _absControl;
                }
                set
                {
                    _absControl = value;
                }
            }

            [XmlElement]
            public float Value
            {
                get
                {
                    return _value;
                }
                set
                {
                    _value = value;
                }
            }
        }
    }
}
