require 'fcntl'
require 'delegate'

module Stty
  VERSION = '1.0.0'

  def self.open filename, speed, mode
    f = File.open filename, File::RDWR|Fcntl::O_NOCTTY|Fcntl::O_NDELAY
    f.sync = true

    # enable blocking reads, otherwise read timeout won't work
    f.fcntl Fcntl::F_SETFL, f.fcntl(Fcntl::F_GETFL, 0) & ~Fcntl::O_NONBLOCK

    t = Termios.new f
    t.flow_control = false

    if mode =~ /^(\d)(\w)(\d)$/
      t.data_bits = $1.to_i
      t.stop_bits = $3.to_i
      t.parity = { 'N' => :none, 'E' => :even, 'O' => :odd }[$2]
      t.speed = speed
      t.read_timeout = 5
      t.reading = true
      t.update!
    end

    if block_given?
      begin
        yield t
      ensure
        t.close
      end
    else
      t
    end
  end

  class Termios < DelegateClass(File)
  end

  require 'stty.so'

  class Termios
    def initialize file
      super(file)
      tcgetattr file
    end

    def reading= val
      if val
        self.c_cflag |= CLOCAL | CREAD
      end
    end

    def nonblocking_reads
      self.c_cflag &= ~ICANON # disable canonical mode
      set_c_cc VTIME, 0
      set_c_cc VMIN, 0
    end

    def blocking_reads
      self.c_cflag &= ~ICANON # disable canonical mode
      set_c_cc VTIME, 0
      set_c_cc VMIN, 1
    end

    def read_timeout= val
      #self.c_cflag &= ~ICANON # disable canonical mode
      set_c_cc VTIME, val
      set_c_cc VMIN, 0
    end

    def c_cc
      19.times.map { |i| get_c_cc i }
    end

    def parity= val
      case val
      when :none
        self.c_cflag &= ~PARENB
      when :even
        self.c_cflag |= PARENB  # Enable parity
        self.c_cflag &= ~PARODD # Make it not odd
      when :odd
        self.c_cflag |= PARENB  # Enable parity
        self.c_cflag |= PARODD  # Make it odd
      else
        raise
      end
    end

    def flow_control= val
      if val
        self.c_iflag |= (IXON | IXOFF | IXANY)
      else
        self.c_iflag &= ~(IXON | IXOFF | IXANY)
      end
    end

    def flush_input
      tcflush __getobj__, TCIFLUSH
    end

    def flush_output
      tcflush __getobj__, TCOFLUSH
    end

    def flush
      tcflush __getobj__, TCIOFLUSH
    end

    def stop_bits= val
      case val
      when 1 then self.c_cflag &= ~CSTOPB
      when 2 then self.c_cflag |= CSTOPB
      else
        raise
      end
    end

    def data_bits= val
      self.c_cflag &= ~CSIZE                        # clear previous values
      self.c_cflag |= Stty.const_get("CS#{val}")    # Set the data bits
    end

    def update!
      tcsetattr __getobj__, TCSANOW
    end

    # Set both input and output speed to +val+
    def speed= val
      cfsetspeed val
    end
  end
end
