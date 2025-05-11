class Process:
    def __init__(self, n, p, a, s):
        self.name = n
        self.priority = int(p)
        self.arrival_time = a
        self.service_time = s
        self.remaining_time = s
        self.isFinished = False

    def toggle_finished(self):
        self.isFinished = True

    def get_status(self):
        return self.isFinished

    def service(self, serviced_time):
        self.remaining_time -= serviced_time
        if self.remaining_time <= 0:
            self.remaining_time = 0
            self.toggle_finished()


class CPU:
    def __init__(self):
        self.current_process = None

    def assign_process(self, process):
        self.current_process = process

    def execute(self):
        if self.current_process:
            self.current_process.service(1)
            if self.current_process.get_status():
                self.current_process = None


# Initialize processes
p1 = Process('p1', 4, 0, 5)
p2 = Process('p2', 1, 2, 3)
p3 = Process('p3', 3, 4, 4)
p4 = Process('p4', 2, 6, 2)
p_list = [p1, p2, p3, p4]

ready_queue = []
cpu = CPU()
time = 0

# Run FCFS simulation
while True:
    # Check for arriving processes
    for process in p_list:
        if process.arrival_time == time and not process.get_status():
            ready_queue.append(process)

    # Assign to CPU if it's idle
    if not cpu.current_process and ready_queue:
        cpu.assign_process(ready_queue.pop(0))

    # Print running process or idle
    if cpu.current_process:
        print(cpu.current_process.name, end=' ')
        cpu.execute()
    else:
        print("idle", end=' ')

    # End if all processes are finished
    if all(p.get_status() for p in p_list):
        print("\nAll processes finished.")
        break

    time += 1
